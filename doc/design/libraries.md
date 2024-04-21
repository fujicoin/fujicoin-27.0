# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libfujicoin_cli*         | RPC client functionality used by *fujicoin-cli* executable |
| *libfujicoin_common*      | Home for common functionality shared by different executables and libraries. Similar to *libfujicoin_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libfujicoin_consensus*   | Stable, backwards-compatible consensus functionality used by *libfujicoin_node* and *libfujicoin_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libfujicoinconsensus*    | Shared library build of static *libfujicoin_consensus* library |
| *libfujicoin_kernel*      | Consensus engine and support library used for validation by *libfujicoin_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libfujicoinqt*           | GUI functionality used by *fujicoin-qt* and *fujicoin-gui* executables |
| *libfujicoin_ipc*         | IPC functionality used by *fujicoin-node*, *fujicoin-wallet*, *fujicoin-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libfujicoin_node*        | P2P and RPC server functionality used by *fujicoind* and *fujicoin-qt* executables. |
| *libfujicoin_util*        | Home for common functionality shared by different executables and libraries. Similar to *libfujicoin_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libfujicoin_wallet*      | Wallet functionality used by *fujicoind* and *fujicoin-wallet* executables. |
| *libfujicoin_wallet_tool* | Lower-level wallet functionality used by *fujicoin-wallet* executable. |
| *libfujicoin_zmq*         | [ZeroMQ](../zmq.md) functionality used by *fujicoind* and *fujicoin-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libfujicoin_consensus* and *libfujicoin_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libfujicoin_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libfujicoin_node* code lives in `src/node/` in the `node::` namespace
  - *libfujicoin_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libfujicoin_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libfujicoin_util* code lives in `src/util/` in the `util::` namespace
  - *libfujicoin_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

fujicoin-cli[fujicoin-cli]-->libfujicoin_cli;

fujicoind[fujicoind]-->libfujicoin_node;
fujicoind[fujicoind]-->libfujicoin_wallet;

fujicoin-qt[fujicoin-qt]-->libfujicoin_node;
fujicoin-qt[fujicoin-qt]-->libfujicoinqt;
fujicoin-qt[fujicoin-qt]-->libfujicoin_wallet;

fujicoin-wallet[fujicoin-wallet]-->libfujicoin_wallet;
fujicoin-wallet[fujicoin-wallet]-->libfujicoin_wallet_tool;

libfujicoin_cli-->libfujicoin_util;
libfujicoin_cli-->libfujicoin_common;

libfujicoin_common-->libfujicoin_consensus;
libfujicoin_common-->libfujicoin_util;

libfujicoin_kernel-->libfujicoin_consensus;
libfujicoin_kernel-->libfujicoin_util;

libfujicoin_node-->libfujicoin_consensus;
libfujicoin_node-->libfujicoin_kernel;
libfujicoin_node-->libfujicoin_common;
libfujicoin_node-->libfujicoin_util;

libfujicoinqt-->libfujicoin_common;
libfujicoinqt-->libfujicoin_util;

libfujicoin_wallet-->libfujicoin_common;
libfujicoin_wallet-->libfujicoin_util;

libfujicoin_wallet_tool-->libfujicoin_wallet;
libfujicoin_wallet_tool-->libfujicoin_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class fujicoin-qt,fujicoind,fujicoin-cli,fujicoin-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libfujicoin_wallet* and *libfujicoin_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libfujicoin_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libfujicoin_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libfujicoin_common* should serve a similar function as *libfujicoin_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libfujicoin_util* and *libfujicoin_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for fujicoin-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libfujicoin_kernel* is not supposed to depend on *libfujicoin_common*, only *libfujicoin_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libfujicoin_kernel* should only depend on *libfujicoin_util* and *libfujicoin_consensus*.

- The only thing that should depend on *libfujicoin_kernel* internally should be *libfujicoin_node*. GUI and wallet libraries *libfujicoinqt* and *libfujicoin_wallet* in particular should not depend on *libfujicoin_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libfujicoin_consensus*, *libfujicoin_common*, and *libfujicoin_util*, instead of *libfujicoin_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libfujicoinqt*, *libfujicoin_node*, *libfujicoin_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libfujicoin_node* to *libfujicoin_kernel* as part of [The libfujicoinkernel Project #24303](https://github.com/fujicoin/fujicoin/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/fujicoin/fujicoin/issues/15732)
