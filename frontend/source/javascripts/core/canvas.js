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
                node : {
                    add : "#3ed221",
                    upd : "#eff128",
                    def : "#aaaaaa",
                    del : "#e84141"
                },
                line : {
                    add : "#3ed221",
                    upd : "#eff128",
                    def : "#aaaaaa",
                    del : "#e84141"
                }
            },
            sizes : {
                node : 10
            },
            timeouts : {
                add : 1000,
                upd : 500,
                del : 1000
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
            color : this.options.colors.node.add,
            size  : this.options.sizes.node
        });
        
        var self = this;
        setTimeout(function () {
            self.graph.addNode(id, {
                color : self.options.colors.node.def
            });
        }, this.options.timeouts.add);
    }
    
    Canvas.prototype.upd_node = function (id) {
        if (id == undefined) {
            return;
        }
        var links_cache = {};
        var links = this.graph.getLinks(id);
        for (var index = 0; index < links.length; index++) {
            var link = links[index];
            var cache_key = link.fromId + " " + link.toId;
            if (links_cache[cache_key]) {
                continue;
            }
            links_cache[cache_key] = true;
            this.upd_link(link.fromId, link.toId);
        }
        
        this.graph.addNode(id, {
            color : this.options.colors.node.upd
        });
        
        var self = this;
        setTimeout(function () {
            self.graph.addNode(id, {
                color : self.options.colors.node.def
            });
        }, this.options.timeouts.upd);
    }
    
    Canvas.prototype.del_node = function (id) {
        if (id == undefined) {
            return;
        }
        
        var links_cache = {};
        var links = this.graph.getLinks(id);
        for (var index = 0; index < links.length; index++) {
            var link = links[index];
            var cache_key = link.fromId + " " + link.toId;
            if (links_cache[cache_key]) {
                continue;
            }
            links_cache[cache_key] = true;
            this.del_link(link.fromId, link.toId);
        }
        
        this.graph.addNode(id, {
            color : this.options.colors.node.del
        });
        
        var self = this;
        setTimeout(function () {
            self.graph.removeNode(id);
        }, this.options.timeouts.del);
    }
    
    Canvas.prototype.add_link = function (id, parent_id) {
        if (id == undefined || parent_id == undefined) {
            return;
        }
        this.graph.addLink(id, parent_id, {
            color : this.options.colors.line.add
        });
        
        var self = this;
        setTimeout(function () {
            self.graph.addLink(id, parent_id, {
                color : self.options.colors.line.def
            });
        }, this.options.timeouts.add);
    }
    
    Canvas.prototype.upd_link = function (id, parent_id) {
        if (id == undefined || parent_id == undefined) {
            return;
        }
        this.graph.addLink(id, parent_id, {
            color : this.options.colors.line.upd
        });
        
        var self = this;
        setTimeout(function () {
            self.graph.addLink(id, parent_id, {
                color : self.options.colors.line.def
            });
        }, this.options.timeouts.upd);
    }
    
    Canvas.prototype.del_link = function (id, parent_id) {
        if (id == undefined || parent_id == undefined) {
            return;
        }
        this.graph.addLink(id, parent_id, {
            color : this.options.colors.line.del
        });
    }
    
    app.Canvas = Canvas;
}) (jQuery)