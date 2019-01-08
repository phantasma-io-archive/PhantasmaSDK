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
	def {{#camel-case Info.Name}}(self{{#each Info.Parameters}}, {{Name}}{{/each}}):	
		"""
		{{Info.Description}}
		{{#if Info.Parameters}}
		Args:{{#each Info.Parameters}}
			{{Name}}: {{Description}}{{/each}}
		{{#else}}
		Args:
			None
		{{/if}}
		"""
		params = [{{#each Info.Parameters}}{{Name}}{{#if !@last}}, {{/if}}{{/each}}];
		return __JSON_RPC(self, "{{#camel-case Info.Name}}", params);	
	{{/each}}
	