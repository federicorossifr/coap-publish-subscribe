from coapthon.resources.resource import Resource
from coapthon import defines
from var_dump import var_dump

def parsePostPayload(payload):
    pairs = payload.split("&");
    payload_dict = {}
    for pair in pairs:
        key_val = pair.split("=")
        payload_dict[key_val[0]] = key_val[1]
        pass
    return payload_dict

class PsResource(Resource):
    def __init__(self, name="PsResource", coap_server=None):
        super(PsResource, self).__init__(name, coap_server, visible=True,
                                            observable=True, allow_children=True)
        self.cs = coap_server
        self.resource_type = "rt1"
        self.content_type = "text/plain"
        self.interface_type = "if1"
        self.payload = "Basic Resource"

    def render_GET_advanced(self, request, response):
        response.payload = self.payload
        response.max_age = 20
        response.code = defines.Codes.CONTENT.number
        return self, response

    def render_POST_advanced(self, request, response):
        payload = parsePostPayload(request.payload)
        print(request.uri_path)
        self.cs.add_resource(request.uri_path+"/"+payload["topic"],PsResource(payload["topic"],self.cs))
        return self,response

    def render_PUT_advanced(self, request, response):
        self.payload = request.payload
        from coapthon.messages.response import Response
        assert(isinstance(response, Response))
        response.payload = "Response changed through PUT"
        response.code = defines.Codes.CHANGED.number
        return self, response

    def render_DELETE_advanced(self, request, response):
        response.payload = "Response deleted"
        response.code = defines.Codes.DELETED.number
        return True, Response