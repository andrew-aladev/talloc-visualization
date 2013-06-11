(function ($, undefined) {
    "use strict";
    
    function Scenario(start_time) {
        this.queue        = [];
        this.current_time = start_time;
        
        this._timer = $.proxy(this.timer, this);
    }
    
    Scenario.types = {
        add  : 0,
        upd  : 1,
        move : 2,
        del  : 3
    }
    
    Scenario.prototype.append_data = function (data) {
        this.queue = this.queue.concat(data);
        if (!this.timeout) {
            this.timer();
        }
    }
    
    Scenario.prototype.timer = function () {
        var now = new Date().getTime();
        while (this.queue.length > 0) {
            var node = this.queue.shift();
            var time = this.current_time + node.time_diff;
            if (time <= now) {
                this.current_time = time;
                this.timeout = null;
                $(app).trigger("node", [node]);
            } else {
                this.queue.unshift(node);
                this.timeout = setTimeout(this._timer, time - now);
                break;
            }
        }
    }
    
    app.Scenario = Scenario;
}) (jQuery)