#pragma once;
#include <vector>
#include <wchar.h>
#include <algorithm>

namespace Interpreter {

enum class Operator : wchar_t {
    Plus = L'+',
    Minus = L'-',
    Mul = L'*',
    Div = L'/',
    LParen = L'(',
    RParen = L')',
};

inline std::wstring ToString(const Operator &op) {
    return{ static_cast<wchar_t>(op) };
}

enum class TokenType {
    Operator,
    Number
};

inline std::wstring ToString(const TokenType &type) {
    switch(type) {
        case TokenType::Operator:
            return L"Operator";
        case TokenType::Number:
            return L"Number";
        default:
            throw std::out_of_range("TokenType");
    }
}

class Token {
public:
    Token(Operator op) :m_type(TokenType::Operator), m_operator(op) {}

    Token(double num) :m_type(TokenType::Number), m_number(num) {}

    TokenType Type() const {
        return m_type;
    }

    operator Operator() const {
        if(m_type != TokenType::Operator) {
            throw std::logic_error("Should be operator token.");
        }
        return m_operator;
    }

    operator double() const {
        if(m_type != TokenType::Number) {
            throw std::logic_error("Should be number token.");
        }
        return m_number;
    }

    friend inline bool operator==(const Token &left, const Token &right) {
        if(left.m_type == right.m_type) {
            switch(left.m_type) {
                case Interpreter::TokenType::Operator:
                    return left.m_operator == right.m_operator;
                case Interpreter::TokenType::Number:
                    return left.m_number == right.m_number;
                default:
                    throw std::out_of_range("TokenType");
            }
        }
        return false;
    }

private:
    TokenType m_type;
    union {
        Operator m_operator;
        double m_number;
    };
};

inline std::wstring ToString(const Token &token) {
    switch(token.Type()) {
        case TokenType::Number:
            return std::to_wstring(static_cast<double>(token));
        case TokenType::Operator:
            return ToString(static_cast<Operator>(token));
        default:
            throw std::out_of_range("TokenType");
    }
}

typedef std::vector<Token> Tokens;

namespace Lexer {

namespace Detail {

class Tokenizer {
public:
    Tokenizer(const std::wstring &expr) : m_current(expr.c_str()) {}

    void Tokenize() {
        while(!EndOfExperssion()) {
            if(IsNumber()) {
                ScanNumber();
            }
            else if(IsOperator()) {
                ScanOperator();
            }
            else {
                MoveNext();
            }
        }
    }

    const Tokens &Result() const {
        return m_result;
    }

private:
    bool EndOfExperssion() const {
        return *m_current == L'\0';
    }

    bool IsNumber() const {
        return iswdigit(*m_current) != 0;
    }

    void ScanNumber() {
        wchar_t *end = nullptr;
        m_result.push_back(wcstod(m_current, &end));
        m_current = end;
    }

    bool IsOperator() const {
        auto all = { Operator::Plus, Operator::Minus, Operator::Mul, Operator::Div, Operator::LParen, Operator::RParen };
        return std::any_of(all.begin(), all.end(), [this](Operator o) {return *m_current == static_cast<wchar_t>(o); });
    }

    void ScanOperator() {
        m_result.push_back(static_cast<Operator>(*m_current));
        MoveNext();
    }

    void MoveNext() {
        ++m_current;
    }

    const wchar_t *m_current;
    Tokens m_result;
};

} // namespace Detail

inline Tokens Tokenize(const std::wstring &expr) {
    Detail::Tokenizer tokenizer(expr);
    tokenizer.Tokenize();
    return tokenizer.Result();
}

} // namespace Lexer

namespace Parser {

inline Tokens Parse(const Tokens &tokens) {
    Tokens output, stack;
    auto popAll = [&]() { while(!stack.empty()) {
        output.push_back(stack.back());
        stack.pop_back();
    }};
    for(const Token &token : tokens) {
        if(token.Type() == TokenType::Operator) {
            popAll();
            stack.push_back(token);
            continue;
        }
        output.push_back(token);
    }
    popAll();
    return output;
}

} // namespace Parser

} // namespace Interpreter