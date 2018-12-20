from coapthon.resources.resource import Resource
from coapthon import defines
import re
def delete_subtree(root_resource,base=False):
    if(len(root_resource.children) == 0):
        print("[BROKER] Deleting: "+root_resource.name)
        root_resource.cs.remove_resource(root_resource.name)
        return
    for l in root_resource.children:
        delete_subtree(l)
        print("[BROKER] Removing from children: "+l.name)
        root_resource.children.remove(l)
    	if(len(root_resource.children) == 0 and not base):
        	print("[BROKER] Deleting: "+root_resource.name)
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
        response.code = defines.Codes.CONTENT.number
        return self, response

    def createResFromPayload(self,payload,base):
        if(payload is None or not re.match(r'^<(\w+)>;(?:(ct=\w+;)|(rt=\w+;)|(if=\w+;)|(sz=\w+;))+$',payload)):
            return None
        payload = payload[:-1]
        topicData = payload.split(";")
        topicPath = topicData[0]
        path = topicPath.replace("<","").replace(">","")
        for res in self.children:
            if(res.name == base+"/"+path):
                #RESROUCE ALREADY EXISTS
                return res
        resource = PsResource(base+"/"+path,self.cs)
        topicData.pop(0);
        attr = {}
        attr["obs"] = ""
        for d in topicData:
            key,val = d.split("=")[0],d.split("=")[1]
            print("[BROKER] Attr: "+key+" Val:"+val)
            if(key == 'ct'):
                val = [val]
                print(val)
            attr[key] = val
        print(attr)
        resource.attributes = attr
        return resource


    def render_POST_advanced(self, request, response):
        child_res = self.createResFromPayload(request.payload,request.uri_path)
        if(child_res is None):
            response.code = defines.Codes.BAD_REQUEST.number
            response.payload = "Bad Request"
            return self,response
        if(child_res in self.children):
            response.code = defines.Codes.FORBIDDEN.number
            response.payload = child_res.name + " Already Exists"
            return self,response
        self.children.append(child_res)
        self.cs.add_resource(child_res.name,child_res)
        response.payload = child_res.name + " Created"
        response.code = defines.Codes.CREATED.number
        print("[BROKER] Resource "+child_res.name+" created.");
        return self,response

    def render_PUT_advanced(self, request, response):
        if(request.uri_path == "ps"):
            response.code = defines.Codes.FORBIDDEN.number
            response.payload = "Forbidden"
            return False, response        
        self.payload = request.payload
        response.payload = "Response changed through PUT"
        response.code = defines.Codes.CHANGED.number
        return self, response

    def render_DELETE_advanced(self, request, response):
        if(request.uri_path == "ps"):
            response.code = defines.Codes.FORBIDDEN.number
            response.payload = "Forbidden"            
            return False, response
        response.payload = "Deleted"
        response.code = defines.Codes.DELETED.number
        print("[BROKER] Deleting subtree of "+self.name)
        if(len(self.children)>0):
            delete_subtree(self,True)
        return True, response