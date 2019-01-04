<?php
class PhantasmaAPI { 
	public $host;

	public function __construct($host) {
		$this->host = $host;
	}    

	public function convertDecimals($amount, $decimals) {
		$mult = pow(10, $decimals);
		$result = sprintf("%.4f", $amount / $mult);
		return $result;
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
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $json);
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);    
		curl_setopt($ch, CURLOPT_HTTPHEADER, array("Accept: application/json"));

		$result = curl_exec($ch); 
		curl_close($ch);
		$output = json_decode($result, true);
		if (array_key_exists('result', $output)){
			$json = json_encode($output['result']);
			return json_decode($json);
		}
		
		return false;
	}
    
	{{#each methods}}
	//{{Info.Description}}
	public function {{#camel-case Info.Name}}({{#each Info.Parameters}}${{Value}}{{#if !@last}}, {{/if}}{{/each}})  
	{	   
		$params = array({{#each Info.Parameters}}${{Value}}{{#if !@last}}, {{/if}}{{/each}});
		return $this->JSONRPC('{{#camel-case Info.Name}}', $params);	
	}
	{{/each}}
} 
?>