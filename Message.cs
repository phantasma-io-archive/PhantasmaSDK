using System;
using System.Collections.Generic;
using System.Text;

using LunarParser;
using LunarParser.XML;
using Ipfs.Api;
using PhantasmaMail.Messages;

namespace PhantasmaMail
{
    public abstract class Message
    {
        public DataNode content { get; private set; }
        public string fromAddress { get; private set; }
        public string toAddress { get; private set; }

        public string hash { get; private set; }

        public Message(DataNode root)
        {
            this.content = root;
            this.fromAddress = root.GetString("from");
            this.toAddress = root.GetString("to");
        }

        /// <summary>
        /// Returns IPFS hash
        /// </summary>
        public string Store()
        {
            if (this.hash != null)
            {
                return this.hash;
            }

            var ipfs = new IpfsClient();
            var text = XMLWriter.WriteToString(this.content);
            var node = ipfs.FileSystem.AddTextAsync(text).Result;
            this.hash = node.Id.Hash.ToBase58();

            return this.hash;
        }

        public static Message FromHash(string hash)
        {
            var xml = PhantasmaMail.Store.GetFile(hash);

           
            if (xml != null)
            {
                var root = XMLReader.ReadFromString(xml);

                root = root.FindNode("mail");
                return new Mail(root);
            }

            return null;            
        }
    }

}
