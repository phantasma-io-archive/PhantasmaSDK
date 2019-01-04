<?php 

/* Very basic implementation of a block explorer using Phantasma. Note that we're using a very primitive approach here to keep the code simple.  */

echo '<h1>Phantasma Mini Explorer</h1>';

include 'C:/Code/PhantasmaSDK/Output/temp/PHP/Libs/Phantasma.php';

$api = new PhantasmaAPI('http://localhost:7077/rpc');

$chains = $api->getChains();
if (!$chains){
	die('could not fetch chains');
}

//var_dump($chains);

echo '<table>
	<tr>
    <th>Name</th>
    <th>Address</th> 
    <th>Transactions</th>
	</tr>
';

foreach ($chains as &$chain) {
	
	echo '<tr>';
    echo "<td>$chain->Name</td>";
    echo "<td>$chain->Address</td>";
    echo "<td>-</td>";
	echo '</tr>'; 
}
echo '</table>';
?>