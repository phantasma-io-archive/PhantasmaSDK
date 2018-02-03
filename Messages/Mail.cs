using LunarParser;
using System.Linq;
using System.Collections.Generic;
using System.Text;

namespace PhantasmaMail.Messages
{
    public class Mail : Message
    {
        public struct Attachment
        {
            public string fileName;
            public string hash;

            public Attachment(string fileName, string hash) : this()
            {
                this.fileName = fileName;
                this.hash = hash;
            }
        }

        public string subject { get; private set; }
        public string body { get; private set; }

        private List<Attachment> _attachments = new List<Attachment>();
        public IEnumerable<Attachment> attachments { get { return _attachments; } }

        public Mail(DataNode content) : base (content)
        {
            this.subject = content.GetString("subject");
            this.body = content.GetString("body");

            this._attachments.Clear();

            var items = content.GetNode("attachments");
            if (items != null)
            {
                foreach (var item in items.Children)
                {
                    var fileName = item.GetString("filename");
                    var hash = item.GetString("hash");
                    var attach = new Attachment(fileName, hash);
                    this._attachments.Add(attach);
                }
            }
        }

        public static Mail Create(Mailbox mailbox, string to, string subject, string body)
        {
            var root = DataNode.CreateObject("mail");
            root.AddField("from", mailbox.name);
            root.AddField("to", to);
            root.AddField("subject", subject);
            root.AddField("body", body);
            return new Mail(root);
        }

    }
}
