#include <iostream>
#include <vector>
#include <string>

#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement *>            StatementList;
typedef std::vector<NExpression *>           ExpressionList;
typedef std::vector<NVariableDeclaration *>  VariableList;

/*
          Documentation of AST and subtypes of each Node-type

Node
|____Expr
|     |____ Int               -> long long value
|     |____ Double            -> double value
|     |____ Identifier        -> string id
|     |____ Method Call       -> string id, ExpressionList args
|     |____ Binary Operator   -> int op, $Expression lhs, $Expression rhs
|     |____ Assignment        -> Identifier& lhs, $Expression rhs
|     |____ Block             -> StatementList statements
|
|____Stmt
|     |____ Expr Stmt         -> $Expression expression
|     |____ Return Stmt       -> $Expression expression
      |____ Variable Decl     -> Identifier& type, Identifier& id, VariableList args
      |____ Function Decl     -> Identifier& type, Identifier& id, VariableList args, NBlock& block
*/

/*
 * --- High Level View of AST ---
 */
class Node {
public:
    virtual ~Node() {}

    virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
};

class NExpression : public Node {
};

class NStatement : public Node {
};

/*
 *  --- Expressions ---
 */
class NInteger : public NExpression {
public:
    long long val;
    NInteger(long long ll) : val(ll) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression {
public:
    double val;
    NDouble(double d) : val(d) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression {
public:
    std::string id;
    NIdentifier(const std::string& id) : id(id) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList     args;

    NMethodCall(const NIdentifier& id, ExpressionList& args) : id(id), args(args) { } /* args */
    NMethodCall(const NIdentifier& id)                       : id(id) { }             /* no args */

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(int op, NExpression& lhs, NExpression& rhs) : op(op), lhs(lhs), rhs(rhs) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : lhs(lhs), rhs(rhs) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

/*
 * --- Statements ---
 */
class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : expression(expression) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement {
public:
    NExpression& expr;
    NReturnStatement(NExpression& expr) : expr(expr) { }

    virtual llvm::Value *codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier&       id;
    NExpression         *assignmentExpr;
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id) : type(type), id(id) { }       /* declaration */
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) : /* declaration with initalizer */
      type(type), id(id), assignmentExpr(assignmentExpr) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList args;
    NBlock& block;
    NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, const VariableList& args, NBlock& block) :
      type(type), id(id), args(args), block(block) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

