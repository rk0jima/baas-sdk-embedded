'use strict';

exports.echoBack = function (event, context) {
    context.logger.info(event);
    context.logger.info(Object.keys(context.clientContext.queryParams).length);

    if (Object.keys(context.clientContext.queryParams).length || (event.toString() != "")) {
        let json = {};
        json.param = context.clientContext.queryParams;
        json.body = event.toString();

        let response = context.apigwResponse(200, context.clientContext.headers, json);

        context.succeed(response);
    } else {
        let response = context.apigwResponse(200, context.clientContext.headers, event);
        
        context.succeed(response);
    }
};

