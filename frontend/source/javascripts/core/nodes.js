(function ($, undefined) {
    "use strict";
    
    function Nodes(canvas, options) {
        this.canvas = canvas;
        this.ids    = {};
        this.options = $.extend({}, options);
        
        this.bind();
    }
    
    Nodes.prototype.bind = function () {
        this._set_node = $.proxy(this.set_node, this);
        
        $(app).bind("node", this._set_node);
    }
    
    Nodes.prototype.set_node = function (event, node) {
        var parent = node.parent;
        var parent_id;
        if (parent) {
            parent_id = this.ids[parent];
        }
        switch (node.type) {
            case app.Scenario.types.add:
                var id = this.ids[node.address] = this.canvas.graph.getNodesCount();
                this.canvas.add_node(id);
                this.canvas.add_link(id, parent_id);
                break;
            case app.Scenario.types.upd:
                this.canvas.upd_node(this.ids[node.address]);
                break;
            case app.Scenario.types.del:
                this.canvas.del_node(this.ids[node.address]);
                break;
        }
    }
    
    app.Nodes = Nodes;
}) (jQuery)