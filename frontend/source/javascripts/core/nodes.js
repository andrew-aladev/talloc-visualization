(function ($, undefined) {
    "use strict";
    
    function Nodes(canvas) {
        this.canvas = canvas;
        this.ids    = {};
        
        this.bind();
    }
    
    Nodes.prototype.bind = function () {
        this._set_node = $.proxy(this.set_node, this);
        
        $(app).bind("node", this._set_node);
    }
    
    Nodes.prototype.set_node = function (event, node) {
        var id = this.ids[node.address] = this.canvas.graph.getNodesCount();
        this.canvas.graph.addNode(id, node);
        
        var parent = node.parent;
        if (parent) {
            var parent_id = this.ids[parent];
            this.canvas.graph.addLink(parent_id, id);
        }
    }
    
    app.Nodes = Nodes;
}) (jQuery)