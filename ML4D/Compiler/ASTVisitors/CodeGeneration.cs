﻿using System;
using System.IO;
using System.Text;
using ML4D.Compiler.Nodes;

namespace ML4D.Compiler.ASTVisitors
{
    public class CodeGeneration : ASTVisitor
    {
        private StringBuilder _FuncPrototypes = new StringBuilder();
        private StringBuilder _MainText = new StringBuilder();
        private StringBuilder _FuncDCLs = new StringBuilder();
        
        private bool InsideFunc { get; set; }

        public void WriteToFile(string fileName)
        {
            string CIncludes = "#include <stdio.h>\n#include <math.h>\n\n";
            string CMainFunction = "int main() {\n";
            
            string programText = CIncludes + _FuncPrototypes + CMainFunction + _MainText + "}\n\n" + _FuncDCLs;

            File.WriteAllText(AppDomain.CurrentDomain.BaseDirectory + fileName + ".c", programText);
        }
        
        // TODO test method, remember to remove.
        public void WriteToConsole(string fileName)
        {
            string programText = "" + _FuncPrototypes + _MainText + _FuncDCLs;
            Console.WriteLine(programText);
        }
        
        private void Emit(string text)
        {
            if (InsideFunc)
                _FuncDCLs.Append(text);
            else
                _MainText.Append(text);
        }
        
        // --- Declarations ---
        public override void Visit(VariableDCLNode node)
        {
            Emit(node.Type + " " + node.ID + " = ");
            base.Visit(node);
            Emit(";\n");
        }

        public override void Visit(FunctionDCLNode node)
        {
            InsideFunc = true;
            
            _FuncPrototypes.Append(node.Type + " " + node.ID + "(");
            Emit(node.Type + " " + node.ID + "(");
            
            foreach (FunctionArgumentNode argumentNode in node.Arguments)
            {
                if (argumentNode != node.Arguments[^1])
                {
                    _FuncPrototypes.Append(argumentNode.Type + " " + argumentNode.ID + ", ");
                    Emit(argumentNode.Type + " " + argumentNode.ID + ", ");
                }
                else
                {
                    _FuncPrototypes.Append(argumentNode.Type + " " + argumentNode.ID);
                    Emit(argumentNode.Type + " " + argumentNode.ID);
                }
            }

            _FuncPrototypes.Append(");\n");
            Emit(") {\n");
            
            Visit(node.Body);
            Emit("\n}\n");
            InsideFunc = false;
        }
        
        // --- Statements ---
        public override void Visit(AssignNode node)
        {
            Emit(node.ID + " = ");
            base.Visit(node);
            Emit(";\n");
        }

        public override void Visit(WhileNode node)
        {
            Emit("while (");
            Visit(node.Predicate);
            Emit(") {\n");
            Visit(node.Body);
            Emit("\n}\n");
        }

        public override void Visit(ReturnNode node)
        {
            if (node.Inner is null)
                Emit("return;");
            
            Emit("return ");
            Visit(node.Inner);
            Emit(";\n");
        }

        public override void Visit(FunctionStmtNode node)
        {
            Emit(node.ID + "(");
            foreach (Node argumentNode in node.Arguments)
            {
                Visit(argumentNode);
                
                if (argumentNode != node.Arguments[^1])
                    Emit(", ");
            }
            Emit(");\n");
        }
        
        public override void Visit(FunctionExprNode node)
        {
            Emit(node.ID + "(");
            foreach (Node argumentNode in node.Arguments)
            {
                Visit(argumentNode);
                
                if (argumentNode != node.Arguments[^1])
                    Emit(", ");
            }
            Emit(")");
            // TODO ret stort problem, ved ikke om ";" eller no. Pga. funcStmt og funcExpr er same
        }

        
        // --- Values ---
        public override void Visit(IDNode node)
        {
            Emit(node.ID + "");
        }

        public override void Visit(DoubleNode node)
        {
            Emit(node.Value + "");
        }

        public override void Visit(IntNode node)
        {
            Emit(node.Value + "");
        }

        public override void Visit(BoolNode node)
        {
            Emit(node.Value ? "1" : "0");
        }


        // --- Arithmetic ---
        public override void Visit(InfixExpressionNode node)
        {
            if (node.Parenthesized)
                Emit("(");
            PrintExpression(node);
            if (node.Parenthesized)
                Emit(")");
        }

        
        
        public override void Visit(UnaryExpressionNode node)
        {
            if (node.Parenthesized)
                Emit("(");
            PrintExpression(node);
            if (node.Parenthesized)
                Emit(")");
        }
        
        private void PrintExpression(InfixExpressionNode node)
        {
            if (node is PowerNode)
            {
                Emit("pow(");
                Visit(node.Left);
                Emit(", ");
                Visit(node.Right);
                Emit(")");
            }
            else
            {
                Visit(node.Left);
                if (node is AndNode || node is OrNode)
                {
                    string symbol = node is AndNode ? "&&" : "||";
                    Emit(" " + symbol + " ");
                } else
                    Emit(" " + node.Symbol + " ");
                Visit(node.Right);
            }
        }
        
        private void PrintExpression(UnaryExpressionNode node)
        {
            if (node is NotNode)
                Emit("!");
            Visit(node.Inner);
        }
    }
}