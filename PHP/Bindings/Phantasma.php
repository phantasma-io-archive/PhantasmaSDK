<?php
class PhantasmaAPI { 
	public $host;

	public function __construct($host) {
		$this->host = $host;
	}    

	private function JSONRPC($method, $params) {
		$message = array();
		$message['jsonrpc'] = '2.0';
		$message['method'] = $method;
		$message['params'] = $params;
		$message['id'] ='1';
		$json = json_encode($message);
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $this->host);
		curl_setopt($ch, CURLOPT_HEADER, false); 
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $json);
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);    
		curl_setopt($ch, CURLOPT_HTTPHEADER, "Accept: application/json");

		$result = curl_exec($ch); 
		curl_close($ch);
		return $result;
	}
    
	{{#each methods}}
	//{{Info.Description}}
	public function {{Info.Name}}({{#each Info.Parameters}}${{Value}}{{#if !@last}}, {{/if}}{{/each}})  
	{	   
		$params = array({{#each Info.Parameters}}${{Value}}{{#if !@last}}, {{/if}}{{/each}});
		return JSONRPC('{{#camel-case Info.Name}}', $params);	
	}
	{{/each}}
} 
?>