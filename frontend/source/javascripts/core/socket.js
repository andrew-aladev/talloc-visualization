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
            this.add_events (event.data);
        } else {
            this.set_options (event.data);
        }
    }
    
    Socket.read_number = function (bytes, from_index, size) {
        var number = 0;
        for (var i = size - 1; i >= 0; i--) {
            number = number * 256 + bytes[from_index + i];
        }
        return number;
    }
    
    Socket.prototype.set_options = function (options) {
        var self     = this;
        this.options = {};
        
        var reader = new FileReader();
        reader.onload = function () {
            var bytes = new Uint8Array(this.result);
            self.options = {
                sizeof_ptr : bytes[0],
                start_time : Socket.read_number(bytes, 1, 8)
            };
            self.init();
        }
        reader.readAsArrayBuffer(options);
    }
    
    Socket.prototype.add_events = function (events) {
        var self = this;
        
        var reader = new FileReader();
        reader.onload = function () {
            self.resolve_event_bytes(new Uint8Array(this.result));
        }
        reader.readAsArrayBuffer(events);
    }
    
    Socket.prototype.resolve_event_bytes = function (bytes) {
        var data = [];
        
        var type, time_diff;
        var address, parent;
        for (var offset = 0; offset < bytes.length; offset += this.packet_size) {
            var read_offset = offset;
            type = bytes[read_offset];
            read_offset++;
            time_diff = Socket.read_number(bytes, read_offset, 4);
            read_offset += 4;
            address = Socket.read_number(bytes, read_offset, this.options.sizeof_ptr);
            read_offset += this.options.sizeof_ptr;
            parent = Socket.read_number(bytes, read_offset, this.options.sizeof_ptr);
            
            data.push({
                type      : type,
                time_diff : time_diff,
                address   : address,
                parent    : parent
            });
        }
        
        app.scenario.append_data(data);
    }
    
    Socket.prototype.init = function () {
        app.scenario     = new app.Scenario(this.options.start_time);
        this.packet_size = 5 + 2 * this.options.sizeof_ptr;
    }
    
    app.Socket = Socket;
}) (jQuery)