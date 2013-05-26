window.app = {};

(function ($, undefined) {
    "use strict";
    
    $(document).ready(function () {
        var container = $(".container");
        app.canvas = new app.Canvas(container);
        app.canvas.run();
        app.nodes = new app.Nodes(app.canvas);
        
        var now = new Date().getTime(); // start time from server
        app.scenario = new app.Scenario(now);
        app.scenario.append_data([
            {
                time_diff : 500,
                address   : 0x00000001,
                parent    : null
            },
            {
                time_diff : 500,
                address   : 0x00000002,
                parent    : 0x00000001
            },
            {
                time_diff : 500,
                address   : 0x00000003,
                parent    : 0x00000002
            },
            {
                time_diff : 500,
                address   : 0x00000004,
                parent    : 0x00000002
            },
            {
                time_diff : 500,
                address   : 0x00000005,
                parent    : 0x00000003
            },
            {
                time_diff : 500,
                address   : 0x00000006,
                parent    : 0x00000003
            }
        ]);
    });
}) (jQuery)