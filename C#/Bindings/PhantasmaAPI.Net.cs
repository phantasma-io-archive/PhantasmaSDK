using System;
using System.Globalization;
using System.Net;
using LunarLabs.Parser;
using LunarLabs.Parser.JSON;
using Phantasma.Numerics;
using Phantasma.Cryptography;

namespace Phantasma.SDK
{
    public enum EPHANTASMA_SDK_ERROR_TYPE
    {
        API_ERROR,
        WEB_REQUEST_ERROR,
        FAILED_PARSING_JSON,
        MALFORMED_RESPONSE
    }

	internal static class APIUtils
    {
        internal static long GetInt64(this DataNode node, string name)
        {
            return node.GetLong(name);
        }

        internal static bool GetBoolean(this DataNode node, string name)
        {
            return node.GetBool(name);
        }
    }

    internal class JSONRPC_Client
    {
        internal void SendRequest(string url, string method, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback, 
                                            Action<DataNode> callback, params object[] parameters)
        {
            var paramData = DataNode.CreateArray("params");
            
            if (parameters!=null && parameters.Length > 0)
            {
                foreach (var obj in parameters)
                {
                    paramData.AddField(null, obj);
                }
            }

            var jsonRpcData = DataNode.CreateObject(null);
            jsonRpcData.AddField("jsonrpc", "2.0");
            jsonRpcData.AddField("method", method);
            jsonRpcData.AddField("id", "1");
            jsonRpcData.AddNode(paramData);
            
            string json;

            try
            {
				json = JSONWriter.WriteToString(jsonRpcData);				
            }
            catch (Exception e)
            {
                throw e;
            }

			string contents;			
			try {
				using (var wc = new WebClient())
				{
					wc.Headers[HttpRequestHeader.ContentType] = "application/json";
					contents = wc.UploadString(url, json);
				}
			}
			catch (Exception e)
			{
				if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.WEB_REQUEST_ERROR, e.Message);
                return;
			}
			
			var root = JSONReader.ReadFromString(contents);
			
			if (root == null)
			{
				if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.FAILED_PARSING_JSON, "failed to parse JSON");
			}
			else 
			if (root.HasNode("error")) {
				var errorDesc = root["error"].GetString("message");
				if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.API_ERROR, errorDesc);
			}
			else
			if (root.HasNode("result"))
			{
				var result = root["result"];
				callback(result);
			}
			else {					
				if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.MALFORMED_RESPONSE, "malformed response");
			}							
        }		
   }
   
   {{#each types}}
	public struct {{#fix-type Key}} 
	{
{{#each Value}}		public {{#fix-type FieldType.Name}}{{#if FieldType.IsArray}}[]{{/if}} {{Name}}; //{{Key.Description}}
{{/each}}	   
		public static {{#fix-type Key}} FromNode(DataNode node) 
		{
			{{#fix-type Key}} result;
{{#each Value}}			{{#if FieldType.IsArray}}
			var {{Name}}_array = node.GetNode("{{#camel-case Name}}");
			if ({{Name}}_array != null) {
				result.{{Name}} = new {{#fix-type FieldType.Name}}[{{Name}}_array.ChildCount];
				for (int i=0; i < {{Name}}_array.ChildCount; i++) {
					{{#if FieldType.Name contains 'Result'}}
					result.{{Name}}[i] = {{#fix-type FieldType.Name}}.FromNode({{Name}}_array.GetNodeByIndex(i));
					{{#else}}						
					result.{{Name}}[i] = {{Name}}_array.GetNodeByIndex(i).As{{#fix-array FieldType.Name}}();{{/if}}
				}
			}
			else {
				result.{{Name}} = new {{#fix-type FieldType.Name}}[0];
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
		//{{Info.Description}}{{#if Info.IsPaginated==true}}{{#new-line}}		//This api call is paginated, multiple calls might be required to obtain a complete result {{/if}}
		public void {{Info.Name}}({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}}, {{/each}}Action<{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}[]{{/if}}{{#if Info.IsPaginated==true}}, int, int{{/if}}> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			_client.SendRequest(Host, "{{#camel-case Info.Name}}", errorHandlingCallback, (node) => { 
{{#parse-lines false}}
{{#if Info.IsPaginated}}
				var currentPage = node.GetInt32("page");{{#new-line}}
				var totalPages = node.GetInt32("totalPages");{{#new-line}}
				node = node.GetNode("result");{{#new-line}}
{{/if}}
{{#if Info.ReturnType.IsPrimitive}}
				var result = {{#fix-type Info.ReturnType.Name}}.Parse(node.Value);
{{#else}}
{{#if Info.ReturnType.Name=='String'}}
				var result = node.Value;
{{#else}}
{{#if Info.ReturnType.IsArray}}
				var result = new {{#fix-type Info.ReturnType.Name}}[node.ChildCount];{{#new-line}}
				for (int i=0; i<result.Length; i++) { {{#new-line}}
					var child = node.GetNodeByIndex(i);{{#new-line}}
					result[i] = {{#fix-type Info.ReturnType.Name}}.FromNode(child);{{#new-line}}
				}
{{#else}}
				var result = {{#fix-type Info.ReturnType.Name}}.FromNode(node);
{{/if}}
{{/if}}
{{/if}}{{#parse-lines true}}
				callback(result{{#if Info.IsPaginated==true}}, currentPage, totalPages{{/if}});
			} {{#each Info.Parameters}}, {{Name}}{{/each}});		   
		}
		
		{{/each}}
		
		public Hash SignAndSendTransaction(KeyPair keys, byte[] script, string chain, Action<string> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)
        {
            var tx = new Blockchain.Transaction("simnet", chain, script, DateTime.UtcNow + TimeSpan.FromHours(1));
            tx.Sign(keys);

            SendRawTransaction(Base16.Encode(tx.ToByteArray(true)), callback, errorHandlingCallback);
            return tx.Hash;
        }

        public static bool IsValidPrivateKey(string address)
        {
            return (address.StartsWith("L", false, CultureInfo.InvariantCulture) ||
                    address.StartsWith("K", false, CultureInfo.InvariantCulture)) && address.Length == 52;
        }

        public static bool IsValidAddress(string address)
        {
            return address.StartsWith("P", false, CultureInfo.InvariantCulture) && address.Length == 45;
        }
	}
}