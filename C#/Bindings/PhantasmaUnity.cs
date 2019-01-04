using System;
using System.Net;
using LunarLabs.Parser;
using LunarLabs.Parser.JSON;

namespace Phantasma.SDK
{
    internal class JSONRPC_Client
    {
        private WebClient client;

        internal JSONRPC_Client()
        {
            client = new WebClient() { Encoding = System.Text.Encoding.UTF8 }; 
        }

        internal DataNode SendRequest(string url, string method, params object[] parameters)
        {
            string contents;

            DataNode paramData;

            if (parameters!=null && parameters.Length > 0)
            {
                paramData = DataNode.CreateArray("params");
                foreach (var obj in parameters)
                {
                    paramData.AddField(null, obj);
                }
            }
            else
            {
                paramData = null;
            }

            var jsonRpcData = DataNode.CreateObject(null);
            jsonRpcData.AddField("jsonrpc", "2.0");
            jsonRpcData.AddField("method", method);
            jsonRpcData.AddField("id", "1");

            if (paramData != null)
            {
                jsonRpcData.AddNode(paramData);
            }

            try
            {
                client.Headers.Add("Content-Type", "application/json-rpc");
				var json = JSONWriter.WriteToString(jsonRpcData);
				contents = client.UploadString(url, json);
            }
            catch (Exception e)
            {
                throw e;
            }

            if (string.IsNullOrEmpty(contents))
            {
                return null;
            }

            //File.WriteAllText("response.json", contents);

            var root = JSONReader.ReadFromString(contents);

            if (root == null)
            {
                return null;
            }

			if (root.HasNode("error")) {
				var errorDesc = node.GetString("error");
				throw new Exception(errorDesc);
			}

            if (root.HasNode("result"))
            {
                return root["result"];
            }

            throw new Exception("malformed response");
        }		
   }
   
   {{#each types}}
	public struct {{Key}} 
	{
{{#each Value}}		public {{FieldType.Name}} {{Name}};{{#new-line}}{{/each}}	   
		public static {{Key}} FromNode(DataNode node) 
		{
			{{Key}} result;
{{#each Value}}			{{#if FieldType.IsArray}}
			var {{Name}}_array = node.GetNode("{{#camel-case Name}}");
			result.{{Name}} = new {{#array-type FieldType.Name}}[{{Name}}_array.ChildCount];
			for (int i=0; i < {{Name}}_array.ChildCount; i++) {
				{{#if FieldType.Name contains 'Result'}}
				result.{{Name}}[i] = {{#array-type FieldType.Name}}.FromNode({{Name}}_array.GetNodeByIndex(i));
				{{#else}}
				result.{{Name}}[i] = {{Name}}_array.GetNodeByIndex(i).As{{#array-type FieldType.Name}}();{{/if}}
			}
			{{#else}}			
			result.{{Name}} = node.Get{{FieldType.Name}}("{{#camel-case Name}}");{{/if}}{{/each}}

			return result;			
		}
	}
	{{/each}}
   
   public class API {	   
		public readonly	string Host;
		private static JSONRPC_Client _client;
	   
		public API(string host) 
		{
			this.Host = host;
			_client = new JSONRPC_Client();
		}
	   
		{{#each methods}}
		//{{Info.Description}}
		public {{Info.ReturnType.Name}} {{Info.Name}}({{#each Info.Parameters}}{{Key.Name}} {{Value}}{{#if !@last}}, {{/if}}{{/each}})  
		{	   
			var node = _client.SendRequest(Host, "{{#camel-case Info.Name}}"{{#each Info.Parameters}}, {{Value}}{{/each}});		   
			return {{Info.ReturnType.Name}}.FromNode(node);		   
		}
		
		{{/each}}
	}
}