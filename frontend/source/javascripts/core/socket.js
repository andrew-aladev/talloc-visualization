(function ($, undefined) {
    "use strict";
    
    function Socket(endpoint) {
        this.endpoint = endpoint;
        this.options  = null;
        
        this.bind();
    }
    
    Socket.prototype.bind = function () {
        this.socket = new WebSocket(this.endpoint);
        
        this._open    = $.proxy(this.open,    this);
        this._close   = $.proxy(this.close,   this);
        this._error   = $.proxy(this.error,   this);
        this._message = $.proxy(this.message, this);
        
        this.socket.onopen    = this._open;
        this.socket.onclose   = this._close;
        this.socket.onerror   = this._error;
        this.socket.onmessage = this._message;
    }
    
    Socket.prototype.open = function (event) {
        if (console) {
            console.log("socket opened", event);
        }
    }
    
    Socket.prototype.close = function (event) {
        if (console) {
            console.log("closed", event);
        }
    }
    
    Socket.prototype.error = function (event) {
        if (console) {
            console.error("error", event);
        }
    }
    
    Socket.prototype.message = function (event) {
        if (this.options) {
            ;
        } else {
            this.set_options (event.data);
        }
    }
    
    Socket.read_number = function (bytes, from_index, size) {
        var number = 0;
        for (var i = 0; i < size; i++) {
            number = number * 256 + bytes[from_index + i];
        }
        return number;
    }
    
    Socket.prototype.set_options = function (options) {
        var self     = this;
        this.options = {};
        
        var reader = new FileReader();
        reader.onload = function (event) {
            var bytes = new Uint8Array(this.result);
            self.options = {
                sizeof_ptr : bytes[0],
                start_time : Socket.read_number(bytes, 1, 8)
            };
            self.init();
        }
        reader.readAsArrayBuffer(options);
    }
    
    Socket.prototype.init = function () {
        app.scenario = new app.Scenario(this.options.start_time);
        
        app.scenario.append_data([
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000001,
                parent    : null
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000002,
                parent    : 0x00000001
            }
        ]);
    }
    
    app.Socket = Socket;
}) (jQuery)