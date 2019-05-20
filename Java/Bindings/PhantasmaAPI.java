import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.type.CollectionType;
import com.github.arteam.simplejsonrpc.client.JsonRpcClient;
import com.github.arteam.simplejsonrpc.client.Transport;
import com.google.common.base.Charsets;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;
import org.jetbrains.annotations.NotNull;
import java.io.IOException;
import java.util.List;

public class PhantasmaAPI {

    final JsonRpcClient client;
    final ObjectMapper mapper;

    public PhantasmaAPI(final String host)
    {
        mapper = new ObjectMapper();
        client = new JsonRpcClient(new Transport() {

            CloseableHttpClient httpClient = HttpClients.createDefault();

            @NotNull
            @Override
            public String pass(@NotNull String request) throws IOException {
                // Used Apache HttpClient 4.3.1 as an example
                HttpPost post = new HttpPost(host+"/rpc");
                post.setEntity(new StringEntity(request, Charsets.UTF_8));
                //post.setHeader(HttpHeaders.ACCEPT, PageAttributes.MediaType.JSON_UTF_8.toString());
                try (CloseableHttpResponse httpResponse = httpClient.execute(post)) {
                    return EntityUtils.toString(httpResponse.getEntity(), Charsets.UTF_8);
                }
            }
        });
    }

	{{#each types}}
	public static class {{#fix-type Key}}{{#parse-lines false}}{{#new-line}}
	{
	{{#new-line}}
	{{#each Value}}
	public {{#if FieldType.IsArray}}List<{{/if}}{{#fix-type FieldType.Name}}{{#if FieldType.IsArray}}>{{/if}} {{Name}};//{{Key.Description}}
	{{#new-line}}
	{{/each}}}
{{#parse-lines true}}
{{/each}}

	{{#each methods}}//{{#if Info.IsPaginated}}Paginated api call: {{/if}}{{Info.Description}}
	public {{#if Info.ReturnType.IsArray}}List<{{/if}}{{#fix-type Info.ReturnType.Name}}{{#if Info.ReturnType.IsArray}}>{{/if}} {{Info.Name}}({{#each Info.Parameters}}{{#fix-type Type.Name}} {{Name}}{{#if !@last}}, {{/if}}{{/each}})
	{
{{#parse-lines false}}
{{#if Info.IsPaginated}}
		Paginated pages = client.createRequest(){{#new-line}}
				.returnAs(Paginated.class){{#new-line}}
{{#else}}
		return client.createRequest(){{#new-line}}
				{{#if Info.ReturnType.IsArray}}.returnAsList({{#else}}.returnAs({{/if}}{{#fix-type Info.ReturnType.Name}}.class){{#new-line}}
{{/if}}
				.method("{{#camel-case Info.Name}}"){{#new-line}}
		        {{#if Info.Parameters}}.params({{#each Info.Parameters}}{{Name}}{{#if !@last}}, {{/if}}{{/each}}){{/if}}{{#new-line}}
				.execute();{{#new-line}}

    {{#if Info.IsPaginated}}
	{{#new-line}}
    	CollectionType pageType = mapper.getTypeFactory().constructCollectionType(List.class, {{#fix-type Info.ReturnType.Name}}.class);{{#new-line}}
        return mapper.convertValue(pages.result, pageType);{{#new-line}}
    {{/if}}
}{{#new-line}}
{{#new-line}}
{{#parse-lines true}}{{/each}}}

