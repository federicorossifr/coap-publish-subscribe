from coapthon.resources.resource import Resource
from coapthon import defines

def delete_subtree(root_resource,base=False):
    if(len(root_resource.children) == 0):
        print("Deleting: "+root_resource.name)
        root_resource.cs.remove_resource(root_resource.name)
        return
    for l in root_resource.children:
        delete_subtree(l)
        print("Removing from children: "+l.name)
        root_resource.children.remove(l)
    	if(len(root_resource.children) == 0 and not base):
        	print("Deleting: "+root_resource.name)
        	root_resource.cs.remove_resource(root_resource.name)
        	return        


def parsePostPayload(payload):
    pairs = payload.split("&");
    payload_dict = {}
    for pair in pairs:
        key_val = pair.split("=")
        payload_dict[key_val[0]] = key_val[1]
        pass
    return payload_dict

class PsResource(Resource):
    def __init__(self, name="PsResource",coap_server=None):
        super(PsResource, self).__init__(name, coap_server, visible=True,
                                            observable=True, allow_children=True)
        self.cs = coap_server
        self.resource_type = "rt1"
        self.content_type = "text/plain"
        self.interface_type = "if1"
        self.payload = "Basic Resource"
        self.children = []

    def render_GET_advanced(self, request, response):
        response.payload = self.payload
        response.max_age = 20
        response.code = defines.Codes.CONTENT.number
        return self, response

    def render_POST_advanced(self, request, response):
        #CHECK PAYLOAD FORMAT
        payload = parsePostPayload(request.payload)
        print(request.uri_path)
        child_res = PsResource(request.uri_path+"/"+payload["topic"],self.cs)
        self.children.append(child_res)
        self.cs.add_resource(request.uri_path+"/"+payload["topic"],child_res)
        response.code = defines.Codes.CREATED.number
        return self,response

    def render_PUT_advanced(self, request, response):
        self.payload = request.payload
        from coapthon.messages.response import Response
        response.payload = "Response changed through PUT"
        response.code = defines.Codes.CHANGED.number
        return self, response

    def render_DELETE_advanced(self, request, response):
        print("Hello");
        #CHECK PAYLOAD FORMAT
        if(request.uri_path == "ps"):
            print("FORBIDDEN")
            response.code = defines.Codes.FORBIDDEN.number
            return False, response
        response.payload = "Response deleted"
        response.code = defines.Codes.DELETED.number
        print("Deleting subtree of "+self.name)
        if(len(self.children)>0):
            delete_subtree(self,True)
        return True, response