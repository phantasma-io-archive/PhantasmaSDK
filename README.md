<p align="center">
  <img
    src="/logo.png"
    width="125px"
  >
</p>

<h1 align="center">Phantasma SDK</h1>

<p align="center">
  Software development kit for decentralized applications using the Phantasma blockchain
</p>

## Contents

- [Description](#description)
- [Languages](#languages)
- [Developing](#developing)
- [Contributing](#contributing)
- [License](#license)

---

## Description

Phantasma implements a decentralized content distribution system running on the blockchain, with strong emphasis on privacy and security.

To learn more about Phantasma, please read the [White Paper](https://phantasma.io/phantasma_whitepaper.pdf) and check the official repository.

## Languages

Click in your language link to get detailed instructions for that specific language and a collection of sample dapps for that language. 

More languages will be available later.

Language 		| Core Library	| Smart Compiler | Sample Dapps
:---------------------- | :------------| :------------| :------------
[.NET / C#](/C#) 		| Beta | In Development | Yes
[PHP](/PHP) 		| Beta | N/A | Yes |
[Python](/Python) 		| Beta | Planned | In Development |
[Golang](/Go) 		| Beta | Planned | In Development |
[Javascript](/JS) 		| Alpha | Planned | In Development |
[C++](/C++) 		| Alpha | Planned | In Development |
[Java](/Java) 		| Alpha | Planned | In Development |

## Developing

To development Phantasma applications it is recommended to run a Phantasma node locally.

Use either the pre-compiled build of Spook which comes bundled in the official SDK release or compile yourself from the source available in the official [repository](https://github.com/phantasma-io/PhantasmaSpook)

To bootstrap your own test net just run a single instance of Phantasma node using the following arguments:
```
Spook.dll -node.wif=L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25 -nexus.name=simnet
```

Note - For a development purposes you can keep your testnet Phantasma network running with just one node. 

You can later move to the official test network where multiple nodes are running, in order to test your dapp under a more realistic enviroment.

## Contributing

You can contribute to Phantasma with [issues](https://github.com/PhantasmaProtocol/PhantasmaChain/issues) and [PRs](https://github.com/PhantasmaProtocol/PhantasmaChain/pulls). Simply filing issues for problems you encounter is a great way to contribute. Contributing implementations is greatly appreciated.

## License

The Phantasma project is released under the MIT license, see `LICENSE.md` for more details.