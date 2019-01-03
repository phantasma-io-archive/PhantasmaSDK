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

            if (root.HasNode("result"))
            {
                return root["result"];
            }

            return root;
        }		
   }
   
   {{#each types}}
   public struct {{Key}} 
   {
	   {{#each Value}}{{FieldType.Name}} {{Name}};
	   {{/each}}
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
		public static {{ReturnType.Name}} {{Name}}({{#each parameters}}{{type}} {{name}},{{/each}})  
		{	   
		   var node = _client.SendRequest(Host, "{{name}}"{{#each parameters}},{{name}}{{/each}})
		}
		
		{{/each}}
   }
}