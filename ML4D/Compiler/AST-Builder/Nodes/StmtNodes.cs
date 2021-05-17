﻿using System.Collections.Generic;
using System.Linq;

namespace ML4D.Compiler.Nodes
{
    public class AssignNode : Node
    {
        public string Type { get; set; }
        public string ID { get; set; }
        public ExpressionNode Right { get; set; }
        
        public AssignNode(string id, ExpressionNode right)
        {
            ID = id;
            Right = right;
        }

        public override List<Node> GetChildren()
        {
            return new List<Node>() {Right};
        }
    }
    
    public class WhileNode : Node
    {
        public ExpressionNode Predicate { get; set; }
        public LinesNode Body { get; set; }
        
        public WhileNode(ExpressionNode predicate, LinesNode body)
        {
            Predicate = predicate;
            Body = body;
        }

        public override List<Node> GetChildren()
        {
            List<Node> children = new List<Node>() {Predicate};
            return children.Concat(Body.lines).ToList();
        }
    }

    public class FunctionStmtNode : FunctionNode
    {
        public FunctionStmtNode(string id) : base(id) {}
    }
    
    public class ReturnNode : Node
    {
        public string Type { get; set; }
        public ExpressionNode? Inner { get; set; }
        public ReturnNode() {}
        public ReturnNode(ExpressionNode inner)
        {
            Inner = inner;
        }
        
        public override List<Node> GetChildren()
        {
            if (Inner is not null)
                return new List<Node>() {Inner};
            return new List<Node>();
        }
    }
    
    // Nyt shit fra 2.0
    public class IfElseNode : Node // TODO, make dis
    {
        public List<IfNode> IfNodes = new List<IfNode>();
        public LinesNode? ElseBody = new LinesNode();
        
        public override List<Node> GetChildren()
        {
            if (ElseBody is null)
            {
                List<Node> children = new List<Node>();
                return children.Concat(IfNodes).ToList();  // TODO tjek den returnere korrekt
            }
            return IfNodes.Concat(ElseBody.lines).ToList(); // TODO tjek den returnere korrekt
        }
    }
    
    public class IfNode : Node 
    {
        public ExpressionNode Predicate { get; set; }
        public LinesNode Body { get; set; }
        
        public override List<Node> GetChildren()
        {
            List<Node> children = new List<Node>() {Predicate};
            return children.Concat(Body.lines).ToList();
        }
    }

    public class ForNode : Node // TODO, make dis
    {
        public AssignNode Init { get; set; }
        public ExpressionNode Predicate { get; set; }
        public AssignNode Final { get; set; }
        public LinesNode Body { get; set; }

        public override List<Node> GetChildren()
        {
            List<Node> children = new List<Node>()
            {
                Init, Predicate, Final
            };
            return children.Concat(Body.lines).ToList();
        }
    }

    public class GradientsNode : Node // TODO, make dis
    {
        public string tensorID { get; set; }
        public List<Node> GradVariables = new List<Node>(); // FunctionArgumentNode
        public List<string> GradTensors = new List<string>();

        public GradientsNode(string tensorId) // Arguments bliver lavet ligesom med funktioner
        {
            tensorID = tensorId;
        }

        public override List<Node> GetChildren()
        {
            return GradVariables;
        }
    }
}