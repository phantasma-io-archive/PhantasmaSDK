package main

import (
	"fmt"
	"math"
	"math/big"
	"phantasma"
)

func main() {
	var host string = "http://127.0.0.1:7077/rpc"

	api := phantasma.NewAPI(host);
	
	account := api.GetAccount("P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr")
	
	fmt.Printf("Got result:)\n")
	fmt.Printf("Account: %s=%s\n", account.Address, account.Name)
	decimals := new(big.Float)
	decimals.SetString(fmt.Sprintf("%f", math.Pow(10, 8)))
	for i := 0; i < len(account.Balances); i++ {
		var balance phantasma.Balance = account.Balances[i]
		i := new(big.Float)
		i.SetString(balance.Amount)
		amount := i.Quo(i, decimals)
		fmt.Printf("Balance on %s: %f %s\n", balance.Chain, amount, balance.Symbol)
	}
	
}
