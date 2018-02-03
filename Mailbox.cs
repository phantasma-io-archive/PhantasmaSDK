using LunarParser;
using LunarParser.XML;
using NeoLux;
using System;
using System.Collections.Generic;
using System.Numerics;

namespace PhantasmaMail
{
    public class Mailbox
    {
        public string name { get; private set; }

        public string address { get
            {
                return keys.address;
            }
        }

        private KeyPair keys;

        private List<Message> _messages = new List<Message>();
        public IEnumerable<Message> messages { get { return _messages; } }

        public Action<Message> onMessageReceived = null;

        public Mailbox(KeyPair keys)
        {
            this.name = null;

            this.keys = keys;

            var script = NeoAPI.GenerateScript(Protocol.scriptHash, "getMailboxFromAddress", new object[] { this.keys.CompressedPublicKey });
            var invoke = NeoAPI.TestInvokeScript(Protocol.net, script);

            if (invoke.result is byte[])
            {
                this.name = System.Text.Encoding.ASCII.GetString((byte[])invoke.result);
            }
        }

        public bool RegisterName(string name)
        {
            if (!string.IsNullOrEmpty(this.name))
            {
                throw new Exception("Name already set");
            }

            var result = NeoAPI.CallContract(NeoAPI.Net.Test, keys, Protocol.scriptHash, "registerMailbox", new object[] { this.keys.CompressedPublicKey, name });

            if (result)
            {
                this.name = name;
            }

            return result;
        }

        public bool SendMessage(Message msg)
        {
            if (msg == null)
            {
                return false;
            }

            var xml = XMLWriter.WriteToString(msg.content);

            var hash = Store.CreateFile(xml);

            return SendMessage(msg.toAddress, hash);
        }

        public bool SendMessage(string destName, string hash)
        {
            if (string.IsNullOrEmpty(destName))
            {
                return false;
            }

            if (string.IsNullOrEmpty(hash))
            {
                return false;
            }

            if (destName.Equals(this.name))
            {
                return false;
            }

            return NeoAPI.CallContract(NeoAPI.Net.Test, keys, Protocol.scriptHash, "sendMessage", new object[] { this.keys.CompressedPublicKey, destName, hash});
        }

        private DateTime lastSync = DateTime.MinValue;

        public bool SyncMessages()
        {
            if (!string.IsNullOrEmpty(this.name))
            {
                var curTime = DateTime.Now;
                var diff = curTime - lastSync;
                if (diff.TotalSeconds<20)
                {
                    return false;
                }

                var script = NeoAPI.GenerateScript(Protocol.scriptHash, "getMailCount", new object[] { System.Text.Encoding.ASCII.GetBytes(this.name) });
                var invoke = NeoAPI.TestInvokeScript(Protocol.net, script);

                if (invoke.result is byte[])
                {
                    var count = new BigInteger((byte[])invoke.result);

                    var oldCount = _messages.Count;
                    for (int i = oldCount + 1; i<= count; i++)
                    {
                        var msg = FetchMessage(i);
                        if (msg != null)
                        {
                            _messages.Add(msg);
                        }
                    }

                    lastSync = curTime;
                    return oldCount != count;
                }
            }

            return false;
        }

        private Message FetchMessage(int index)
        {
            var script = NeoAPI.GenerateScript(Protocol.scriptHash, "getMailContent", new object[] { this.name, index });
            var invoke = NeoAPI.TestInvokeScript(Protocol.net, script);

            if (invoke.result is byte[])
            {
                var hash = System.Text.Encoding.ASCII.GetString((byte[])invoke.result);

                var msg = Message.FromHash(hash);
                return msg;
            }

            return null;
        }
    }
}
