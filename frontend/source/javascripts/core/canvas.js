(function ($, undefined) {
    "use strict";
    
    function Canvas(el, options) {
        var defaults = {
            layout : {
                springLength : 50,
                springCoeff  : 0.0008,
                dragCoeff    : 0.00,
                gravity      : -0.2,
                theta        : 1
            },
            render : {
                renderLinks : true,
                prerender   : true
            }
        };
        this.options = $.extend(defaults, options);
        this.el = el;
        
        this.init();
    }
    
    Canvas.prototype.init = function () {
        this.graph    = Viva.Graph.graph();
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
    
    app.Canvas = Canvas;
}) (jQuery)