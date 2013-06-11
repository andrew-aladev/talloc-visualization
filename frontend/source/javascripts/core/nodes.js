(function ($, undefined) {
    "use strict";
    
    function Nodes(canvas, options) {
        this.canvas  = canvas;
        this.max_id  = 1;
        this.ids     = {};
        this.parents = {};
        this.options = $.extend({}, options);
        
        this.bind();
    }
    
    Nodes.prototype.bind = function () {
        this._set_node = $.proxy(this.set_node, this);
        
        $(app).bind("node", this._set_node);
    }
    
    Nodes.prototype.set_node = function (event, node) {
        var address = node.address;
        
        var parent = node.parent;
        var parent_id;
        if (parent) {
            parent_id = this.ids[parent];
        }
        
        switch (node.type) {
            case app.Scenario.types.add:
                var id = this.ids[address] = this.max_id;
                this.max_id++;
                this.canvas.add_node(id);
                this.canvas.add_link(id, parent_id);
                this.parents[id] = parent_id;
                break;
            case app.Scenario.types.upd:
                break;
            case app.Scenario.types.move:
                var id = this.ids[address];
                var old_parent_id = this.parents[id];
                this.canvas.del_link(id, old_parent_id);
                this.canvas.add_link(id, parent_id);
                this.parents[id] = parent_id;
                break;
            case app.Scenario.types.del:
                this.canvas.del_node(this.ids[address]);
                break;
        }
    }
    
    app.Nodes = Nodes;
}) (jQuery)