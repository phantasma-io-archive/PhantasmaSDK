class PhantasmaAPI {

	const Opcodes = Object.freeze({
		{{#each opcodes}}{{#upper-case Key}}:   {{Value}},
		{{/each}}
	});

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
		console.log(json);
		
		let request = new XMLHttpRequest();

		request.open('POST', this.host, true);

		request.onreadystatechange = function() {
			console.log(this.readyState);
			if (this.readyState === 4) { 
				if (this.status === 200) {
					let data = JSON.parse(request.responseText);
					if (data.result) {
						onSuccess(data.result);
					}
					else {
						onError('invalid response');				
					}
				}
				else {
					onError('connection error');				
				}
			} else {
				return true;
			}		  			
		};

		request.onerror = function() {
			onError('internal error');
		};

		request.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		request.setRequestHeader('Accept', 'application/json'); 
		request.send(json);
		console.log("sent");
	}

	convertDecimals(amount, decimals) {
		let mult = Math.pow(10, decimals);
		return amount / mult;
	}

	{{#each methods}}
	//{{Info.Description}}
	{{#camel-case Info.Name}}({{#each Info.Parameters}}{{Name}},{{/each}}onSuccess, onError)
	{
		let params = [{{#each Info.Parameters}}{{Name}}{{#if !@last}}, {{/if}}{{/each}}];
		this.JSONRPC('{{#camel-case Info.Name}}', params, onSuccess, onError);
	}
{{/each}}}