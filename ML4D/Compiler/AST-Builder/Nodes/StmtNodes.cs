﻿using System.Collections.Generic;

namespace ML4D.Compiler
{
    public class AssignNode : Node
    {
        //public string Type { get; set; } //TODO ikke brugt pt, men jeg tænker nødvendigt for type checking.
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
            List<Node> children = new List<Node>();
            children.Add(Predicate);
            foreach (Node node in Body.lines)
                children.Add(node);
            return children;
        }
    }
    
    public class BackwardNode : Node // TODO slet
    {
        public string ID { get; set; }
        public BackwardNode(string id)
        { 
            ID = id;
        }

        public override List<Node> GetChildren() // Gotta have it
        { return new List<Node>(); }
    }
    
    public class ReturnNode : Node // TODO dilemma ift. om man skal arve fra unaryexpression node eller ikke.
    {
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
}