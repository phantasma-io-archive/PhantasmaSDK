class PhantasmaAPI {

	constructor(host) {
		this.host = host;
	}

	JSONRPC(method, params, onSuccess, onError) {
		let message = {
			'jsonrpc': '2.0',
			'method': method,
			'params': params,
			'id': '1'
		};
		let json = JSON.stringify(message);
		
		let request = new XMLHttpRequest();
		request.open('POST', this.host, true);
		request.setRequestHeader('Content-Type', 'application/json');

		request.onload = function() {
		  if (request.status >= 200 && request.status < 400) {
			let data = JSON.parse(request.responseText);
			onSuccess(data);
		  } else {
			  onError('request error: '+ request.status);
			// We reached our target server, but it returned an error
		  }
		};

		request.onerror = function() {
			onError('internal error');
		};

		request.send();
	}
  
	{{#each methods}}
	//{{Info.Description}}
	{{#camel-case Info.Name}}({{#each Info.Parameters}}{{Name}}, {{/each}}onSuccess, onError)  
	{	   
		let params = [{{#each Info.Parameters}}{{Name}}{{#if !@last}}, {{/if}}{{/each}}];
		this.JSONRPC('{{#camel-case Info.Name}}', params, onSuccess, onError);	
	}
	{{/each}}  
}
