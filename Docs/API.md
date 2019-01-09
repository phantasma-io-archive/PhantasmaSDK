## JSON-RPC methods
{{#each methods}}
* [{{#camel-case Info.Name}}](#{{#camel-case Info.Name}}){{/each}}

## JSON RPC API Reference

***

{{#each methods}}
#### {{#camel-case Info.Name}}
{{Info.Description}}


##### Parameters

{{#if Info.Parameters}}{{#each Info.Parameters}}{{@index+1}}. `{{#fix-type Type.Name}}` {{Description}}.{{/each}}
{{#else}}
none
{{/if}}

```js
params: [
   'PDHcAHq1fZXuwDrtJGghjemFnj2ZaFc7iu3qD4XjZG9eV'
]
```

##### Returns

`{{#fix-type Info.ReturnType.Name}}` - A {{#fix-type Info.ReturnType.Name}}{{#if !Info.ReturnType.IsPrimitive}} object{{/if}}{{#if Info.FailCases}}, or `error` if {{#each Info.FailCases}}{{Description}}{{#if !@last}}or {{/if}}{{/each}}{{/if}}.

  - `address `: `string` - Given address.
  - `name`: `string` - Name of given address.
  - `balances`: `Array` - Array of balance objects.
  - `balance - chain`: `string` - Name of the chain.
  - `balance - symbol`: `string` - Token symbol.
  - `balance - amount`: `string` - Amount of tokens.
 
  
##### Example
```js
// Request
curl -X POST --data '{"jsonrpc":"2.0","method":"{{#camel-case Info.Name}}","params":[{{#each Info.Parameters}}"{{Value}}"{{#if !@last}}, {{/if}}{{/each}}],"id":1}'

// Result
{
   "jsonrpc":"2.0",
   "result":{
      "address":"P16m9XNDHxUex9hsGRytzhSj58k6W7BT5Xsvs3tHjJUkX",
      "name":"genesis",
      "balances":[
         {
            "chain":"main",
            "amount":"511567851650398",
            "symbol":"SOUL"
         },
         {
            "chain":"apps",
            "amount":"891917855944784",
            "symbol":"SOUL"
         }
      ]
   },
   "id":"1"
}
```

***

{{/each}}