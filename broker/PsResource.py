from coapthon.resources.resource import Resource
from coapthon import defines
import re
pattern = r'^<topic>;(?:(ct=\w+;)|(rt=\w+;)|(rel=\w+;)|(hreflang=\w+;)|(media=\w+;)|(title=\w+;)|(if=\w+;))+$'
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




class PsResource(Resource):
    def __init__(self, name="PsResource",coap_server=None):
        super(PsResource, self).__init__(name, coap_server, visible=True,
                                            observable=True, allow_children=True)
        self.cs = coap_server
        self.resource_type = "core.ps"
        self.content_type = "text/plain"
        self.payload = ""
        self.children = []

    def render_GET_advanced(self, request, response):
        response.payload = self.payload
        response.max_age = 20
        response.code = defines.Codes.CONTENT.number
        return self, response

    def createResFromPayload(self,payload,base):
        if(payload is None or not re.match(r'^<(\w+)>;(?:(ct=\w+;)|(rt=\w+;)|(rel=\w+;)|(hreflang=\w+;)|(media=\w+;)|(title=\w+;)|(if=\w+;))+$',payload)):
            return None
        payload = payload[:-1]
        topicData = payload.split(";")
        topicPath = topicData[0]
        path = topicPath.replace("<","").replace(">","")
        resource = PsResource(base+"/"+path,self.cs)
        topicData.pop(0);
        for d in topicData:
            keyval = d.split("=")
            print(keyval)
            resource._attributes[keyval[0]] = keyval[1]
        return resource


    def render_POST_advanced(self, request, response):
        child_res = self.createResFromPayload(request.payload,request.uri_path)
        if(child_res is None):
            response.code = defines.Codes.BAD_REQUEST.number
            return self,response
        self.children.append(child_res)
        print(child_res.name)   
        self.cs.add_resource(child_res.name,child_res)
        response.code = defines.Codes.CREATED.number
        return self,response

    def render_PUT_advanced(self, request, response):
        if(request.uri_path == "ps"):
            print("FORBIDDEN")
            response.code = defines.Codes.FORBIDDEN.number
            return False, response        
        self.payload = request.payload
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