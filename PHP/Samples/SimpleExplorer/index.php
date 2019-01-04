<?php 

/* Very basic implementation of a block explorer using Phantasma. Note that we're using a very primitive approach here to keep the code simple.  */

echo '<h1>Phantasma Mini Explorer</h1>';

// TODO remove hardcoded path
include 'C:/Code/PhantasmaSDK/Output/temp/PHP/Libs/Phantasma.php';

$api = new PhantasmaAPI('http://localhost:7077/rpc');

$tokenList = $api->getTokens();
if (!$tokenList) {
	die("failed to fetch tokens");
}

// map token list to assoc array indexed with token symbols
$tokens = array();
foreach ($tokenList as &$token) {	
	$tokens[$token->Symbol] = $token;
}

if (isset($_GET['address'])) {
	// show info about address
	$address = $_GET['address'];
	echo "<h3>Address $address</h3>";
	
	$account = $api->getAccount($address);	
	//var_dump($account);
	
	echo "<h5>Name: $account->Name</h5>";
	echo '<table border="1">
		<tr>
		<th>Chain</th> 
		<th>Token</th>
		<th>Amount</th>
		</tr>
	';

	// 8 decimal places, note that in a real application this should not be hardcoded value, since every token might have different decimal precision
	$mult = 100000000; 
	
	foreach ($account->Balances as &$balance) {	
		$token = $tokens[$balance->Symbol];
		$amount = $api->convertDecimals($balance->Amount, $token->Decimals);
		echo '<tr>';
		echo "<td><a href='index.php?chain=$balance->Chain'>$balance->Chain</a></td>";
		echo "<td><a href='index.php?token=$balance->Symbol'>$balance->Symbol</a></td>";
		echo "<td><a href='index.php?token=$amount'>$amount</a></td>";
		echo '</tr>'; 
	}
	echo '</table>';	
	
}
else 
if (isset($_GET['chain'])) {
}
else 
if (isset($_GET['token'])) {
}
else
{	// default to showing list of available sidechains
	echo '<h3>Chain List</h3>';
	
	$chains = $api->getChains();
	
	if (!$chains){
		die('could not fetch chains');
	}
	
	//var_dump($chains);
	echo '<table border="1">
		<tr>
		<th>Name</th>
		<th>Address</th> 
		<th>Height</th>
		</tr>
	';

	foreach ($chains as &$chain) {	
		echo '<tr>';
		echo "<td><a href='index.php?chain=$chain->Name'>$chain->Name</a></td>";
		echo "<td><a href='index.php?address=$chain->Address'>$chain->Address</a></td>";
		echo "<td>$chain->Height</td>";
		echo '</tr>'; 
	}
	echo '</table>';	
}

?>