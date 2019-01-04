import requests
import json

class PhantasmaAPI:
	def __init__(self, host):
		self.host = host
	
	def __JSON_RPC(self, method, params):
		headers = {'content-type': 'application/json'}

		payload = {
			"method": method,
			"params": params,
			"jsonrpc": "2.0",
			"id": 1,
		}
		response = requests.post(url, data=json.dumps(payload), headers=headers).json()

		assert response["jsonrpc"]
		assert response["id"] == 1
		return response["result"]

{{#each methods}}
	'''{{Info.Description}}'''
	def {{#camel-case Info.Name}}(self{{#each Info.Parameters}}, {{Value}}{{/each}}):	
		params = [{{#each Info.Parameters}}{{Value}}{{#if !@last}}, {{/if}}{{/each}}];
		return __JSON_RPC(self, "{{#camel-case Info.Name}}", params);	
	{{/each}}
	