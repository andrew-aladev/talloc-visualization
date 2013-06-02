window.app = {};

(function ($, undefined) {
    "use strict";
    
    $(document).ready(function () {
        app.canvas = new app.Canvas($("body"));
        app.canvas.run();
        app.nodes = new app.Nodes(app.canvas);
        
        var now = new Date().getTime(); // start time from server
        app.scenario = new app.Scenario(now);
        
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
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000003,
                parent    : 0x00000002
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000004,
                parent    : 0x00000002
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000005,
                parent    : 0x00000003
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000006,
                parent    : 0x00000003
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000007,
                parent    : 0x00000003
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000008,
                parent    : 0x00000003
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000009,
                parent    : 0x00000002
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000010,
                parent    : 0x00000009
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000011,
                parent    : 0x00000009
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000012,
                parent    : 0x00000001
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000012,
                parent    : 0x00000001
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000013,
                parent    : 0x00000001
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000014,
                parent    : 0x00000013
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000015,
                parent    : 0x00000014
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000016,
                parent    : 0x00000013
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000017,
                parent    : 0x00000014
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000018,
                parent    : 0x00000014
            },
            {
                type      : app.Scenario.types.add,
                time_diff : 500,
                address   : 0x00000019,
                parent    : 0x00000018
            },
            {
                type      : app.Scenario.types.upd,
                time_diff : 1000,
                address   : 0x00000003,
                parent    : null
            },
            {
                type      : app.Scenario.types.upd,
                time_diff : 1000,
                address   : 0x00000005,
                parent    : null
            },
            {
                type      : app.Scenario.types.upd,
                time_diff : 1000,
                address   : 0x00000010,
                parent    : null
            },
            {
                type      : app.Scenario.types.upd,
                time_diff : 1000,
                address   : 0x00000011,
                parent    : null
            },
            {
                type      : app.Scenario.types.del,
                time_diff : 1000,
                address   : 0x00000003,
                parent    : null
            },
            {
                type      : app.Scenario.types.del,
                time_diff : 1000,
                address   : 0x00000001,
                parent    : null
            },
            {
                type      : app.Scenario.types.del,
                time_diff : 1000,
                address   : 0x00000005,
                parent    : null
            },
            {
                type      : app.Scenario.types.del,
                time_diff : 1000,
                address   : 0x00000002,
                parent    : null
            },
            {
                type      : app.Scenario.types.del,
                time_diff : 1000,
                address   : 0x00000007,
                parent    : null
            }
        ]);
    });
}) (jQuery)