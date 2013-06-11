(function ($, undefined) {
    "use strict";
    
    function Canvas(el, options) {
        var defaults = {
            layout : {
                springLength : 50,
                springCoeff  : 0.0008,
                dragCoeff    : 0.00,
                gravity      : -0.5,
                theta        : 1
            },
            render : {
                renderLinks : true,
                prerender   : true
            },
            colors : {
                node : "#aaaaaa",
                line : "#aaaaaa"
            },
            sizes : {
                node : 10
            }
        };
        this.options = $.extend(defaults, options);
        this.el = el;
        
        this.init();
    }
    
    Canvas.prototype.init = function () {
        this.graph    = Viva.Graph.graph();
        // this.graph    = Viva.Graph.generator().balancedBinTree(8);
        this.layout   = Viva.Graph.Layout.forceDirected(this.graph, this.options.layout);
        this.graphics = Viva.Graph.View.webglGraphics();
        
        this.options.render.container = this.el.get(0);
        this.options.render.layout    = this.layout;
        this.options.render.graphics  = this.graphics;
        this.renderer = Viva.Graph.View.renderer(this.graph, this.options.render);
    }
    
    Canvas.prototype.run = function () {
        this.renderer.run();
    }
    
    Canvas.prototype.add_node = function (id) {
        if (id == undefined) {
            return;
        }
        this.graph.addNode(id, {
            color : this.options.colors.node,
            size  : this.options.sizes.node
        });
    }
    
    Canvas.prototype.del_node = function (id) {
        if (id == undefined) {
            return;
        }
        
        this.graph.removeNode(id);
    }
    
    Canvas.prototype.add_link = function (id, parent_id) {
        if (id == undefined || parent_id == undefined) {
            return;
        }
        this.graph.addLink(id, parent_id, {
            color : this.options.colors.line
        });
    }
    
    Canvas.prototype.del_link = function (id, parent_id) {
        if (id == undefined || parent_id == undefined) {
            return;
        }
        this.graph.addLink(id, parent_id, {
            color : this.options.colors.line
        });
    }
    
    app.Canvas = Canvas;
}) (jQuery)