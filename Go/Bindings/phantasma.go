package phantasma

import (
	"fmt"
	"log"
	"github.com/ybbus/jsonrpc"
)

{{#each types}}
type {{#fix-type Key}} struct 
{
{{#each Value}} {{Name}} {{#if FieldType.IsArray}}[]{{/if}}{{#fix-type FieldType.Name}} `json:"{{Name}}"`{{#new-line}}{{/each}}	   
}
{{/each}}

type phantasma struct {
	host string
	rpcClient jsonrpc.RPCClient
}

func NewAPI(host string) *phantasma {
	fmt.Printf("Connecting to %s\n", host)
    api := new(phantasma)
    api.host = host
    api.rpcClient = jsonrpc.NewClient(host)
    return api
}

{{#each methods}}
//{{Info.Description}}
func (api phantasma) {{Info.Name}}({{#each Info.Parameters}}{{Name}} {{#fix-type Type.Name}}{{#if !@last}}, {{/if}}{{/each}}) {{#if Info.ReturnType.IsArray}}[]{{/if}}*{{#fix-type Info.ReturnType.Name}} {
    var result {{#if Info.ReturnType.IsArray}}[]{{/if}}*{{#fix-type Info.ReturnType.Name}}
    err := api.rpcClient.CallFor(&result, "{{#camel-case Info.Name}}"{{#each Info.Parameters}}, {{Name}}{{/each}})
    if err != nil {
		log.Fatal(err)
    }

	return result
}
{{/each}}