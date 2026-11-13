# Istio

ServiceMesh 是一种强大微服务架构，但并不是一个架构产品，比较受欢迎的 ServiceMesh 产品有 Istio、Linkerd 和 Consul Connect 等。

其中的 Istio 是最受欢迎、最强大、最值得信赖的服务网格。 Istio 由 Google、IBM 和 Lyft 于 2016 年创立，是云原生计算基金会的一个毕业项目， 与 Kubernetes 和 Prometheus 等项目并列。

Istio 可确保云原生和分布式系统具有弹性，帮助现代企业在保持连接和保护的同时跨不同平台维护其工作负载。 它启用安全和治理控制，包括 mTLS 加密、策略管理和访问控制、 支持网络功能，例如金丝雀部署、A/B 测试、负载平衡、故障恢复， 并增加对整个资产流量的可观察性。

Istio 并不局限于单个集群、网络或运行时的边界——在 Kubernetes 或 VM、多云、混合或本地上运行的服务都可以包含在单个网格中。

Istio 经过精心设计，具有可扩展性，并受到贡献者和合作伙伴的广泛生态系统的支持， 它为各种用例提供打包的集成和分发。您可以独立安装 Istio，也可以选择由提供基于 Istio 的解决方案的商业供应商提供的托管支持。

![image](https://img2024.cnblogs.com/blog/167509/202409/167509-20240907145057473-1862259619.png)

官方网站：https://istio.io/latest/

### 5.1 Istio 部署逻辑

Istio 服务网格在部署逻辑上分为数据平面和控制平面。

- 数据平面是一组代理，用于调解和控制微服务之间的所有网络通信。 它们还收集和报告所有网格流量的可观测数据。
- 控制平面管理和配置数据平面中的代理。

**Istio 支持两种主要的数据平面模式：**

1. **Sidecar 模式，** 它会与您在集群中启动的每个 Pod 一起部署一个 Envoy 代理，或者与在虚拟机上运行的服务一同运行。
2. **Ambient 模式，** 使用每个节点的四层代理，并且可选地使用每个命名空间的 Envoy 代理来实现七层功能。

### 5.2 Ambient 模式

**Ambient 模式**可以理解成：**不再给每个 Pod 塞一个 Sidecar，而是把服务网格代理能力下沉到 node 和命名空间这一层。**

所以它经常也被称为 **“sidecar-less mesh（无 Sidecar 服务网格）”**。Istio 官方现在把 Ambient 的能力分成两层：底层是每个节点上的 `ztunnel`，负责 L3/L4；需要七层能力时，再额外部署 Waypoint Proxy。([Istio](https://istio.io/latest/docs/ambient/overview/?utm_source=chatgpt.com))

对比一下就很清楚

**Sidecar 模式：**

```text
Pod A
├── App
└── Envoy Sidecar

Pod B
├── App
└── Envoy Sidecar
```

基本上每个 Pod 都带一个 Envoy。

流量大致是：

```text
App A
  ↓
Envoy A
  ↓
网络
  ↓
Envoy B
  ↓
App B
```

优点是每个工作负载旁边都有完整的 L7 代理能力，但 Pod 数量多以后，Envoy 实例也会很多。

------

**Ambient 模式：**

Pod 自己里面没有 Envoy：

```text
Node 1
├── Pod A
├── Pod B
└── ztunnel

Node 2
├── Pod C
├── Pod D
└── ztunnel
```

每个 Kubernetes Node 运行一个共享的 **ztunnel**。

ztunnel 主要负责：

- mTLS 加密
- 工作负载身份认证
- L4 授权
- TCP 层遥测
- 安全隧道传输

它不会去解析 HTTP Header，也不会承担复杂的 HTTP 路由。([Istio](https://istio.io/latest/docs/ambient/overview/?utm_source=chatgpt.com))

那 HTTP 路由、重试、限流这些怎么办？

这就是第二层：

```text
           Waypoint Proxy
              (Envoy)
                 ↑
                 │ L7
                 │
ztunnel ─────────┴───────── ztunnel
   ↑                         ↑
 Pod A                     Pod B
```

如果某个命名空间或工作负载需要：

- HTTP 路由
- HTTP 负载均衡
- Retry / Timeout
- 熔断
- 限流
- 基于 HTTP Header 的授权
- HTTP Metrics
- Trace

就部署一个 **Waypoint Proxy**。

Waypoint 本质上还是 Envoy，但它**不是塞到每个 Pod 里，而是在 Pod 外部共享使用**。([Istio](https://istio.io/latest/docs/ambient/usage/waypoint/?utm_source=chatgpt.com))

所以你看到的这句话：

> “使用每个节点的四层代理，并且可选地使用每个命名空间的 Envoy 代理来实现七层功能。”

可以浓缩成：

```text
Ambient Mesh

L7（可选）
┌─────────────────────────┐
│ Waypoint / Envoy        │
│ HTTP 路由、重试、限流等  │
└─────────────────────────┘
             ↑
L4（默认）
┌─────────────────────────┐
│ ztunnel                  │
│ mTLS、身份、TCP、安全     │
└─────────────────────────┘
             ↑
┌─────────────────────────┐
│ Application Pod         │
│ 不需要 Sidecar Envoy     │
└─────────────────────────┘
```

最关键的区别可以记成一句话：

**Sidecar = 每个 Pod 一个 Envoy；Ambient = 每个 Node 一个轻量 ztunnel，需要 L7 时再共享 Waypoint Envoy。**

因此 `Ambient` 在这里可以理解成一种“**基础设施环境级代理**”的思想：代理能力存在于应用周围的基础设施中，而不是侵入每个应用 Pod。官方也强调，这种分层方式允许你先只启用 L4 安全能力，之后有需要再增加 L7，而不必一开始就给所有 Pod 配完整 Envoy。([Istio](https://istio.io/latest/docs/ambient/overview/?utm_source=chatgpt.com))

### 5.3 Istio 架构图

Istio 架构图如下：
![image](https://img2024.cnblogs.com/blog/167509/202409/167509-20240907151534228-1152738965.png)

它提供了如下功能：

- 针对 HTTP，gRPC，WebSocket 和 TCP 协议提供负载均衡；
- 精细的流量控制，比如 A/B 测试，金丝雀部署等；
- 模块化的插件设计，可以通过 API 进行访问，频率限制等；
- 全自动的请求遥测，包括请求的追踪，监控和日志；
- 强大的安全功能，比如认证，授权，加密等；

**总之：** `Discovery`、`Configuration`、`Certificates`

### 5.4 控制面

控制面负责服务治理，比如路由规则，流量规则（异常重试、超时重试、超时保护、限流、熔断），安全策略等，是服务网格的控制核心，通过控制面，可以配置服务网格中各个组件的行为。

为了结构化控制面的功能，Istio 将其分为 `Pilot`，`Mixer`，`Citadel` 等组件，其各个部分对应的功能：

- **Pilot：**负责服务发现，负载均衡，路由规则等，不过 Pilot 不提供服务注册，只提供标准化的接口，可以方便的对接到各个服务注册中心，比如 Eureka，Etcd 等，然后通过服务发现控制 Envoy 的动态转发能力；
- **Mixer：**负责访问控制，策略执行等，在最初的 Istio 的架构设计中，Mixer 是中心化的组件，由于 Mixer 提供了各种访问控制策略，所以 Mixer 的负载压力比较大，发起请求之前做一次逻辑检查，请求结束后还需要上报处理，Mixer 接收的请求至少涨了原始请求的 2 倍。为了解决这个问题，Mixer 增加了缓存的功能，逻辑处理和上报都由 Mixer 缓存完成，这样 Mixer 的负载压力就能缓解；
- **Citadel：**负责安全功能，比如认证授权等，比如那些服务安全级别比较高，需要对请求做单独的加密处理或者角色控制，Istio 通过引入 Citadel 组件，将安全能力透明化；

> 在早期的 Istio（1.5 版本之前），控制面（Control Plane）自身也是微服务架构，Citadel 是一个独立运行的进程模块。 但后来官方发现，控制面拆得太散反而严重增加了运维负担。于是，Istio 团队“返璞归真”，把负责安全的 Citadel、负责路由的 Pilot、负责配置的 Galley 等组件，全部合并打包成了一个统一的**大单体进程——Istiod**。

### 5.5 数据面

可以看到架构图上，每个服务都有一个 sidecar，也就是  Envoy，这个就是数据面，负责服务间通讯和负载均衡。所有进入服务的请求都经过 Envoy，然后根据路由规则转发到相应的服务，所以 Envoy 被称为服务网格的入口。

Envoy 架构图如下：
![image](https://img2024.cnblogs.com/blog/167509/202409/167509-20240907160054934-1641957878.png)

Envoy 包括几个部分：

- listeners：监听器，负责监听端口，接收请求，比如上述的配置文件中监听 10000 端口；
- filter Chains：过滤器链，可以在配置文件配置对于请求的处理链路，可以在任何一个套接字上，按我们的需要去拼接多个过滤器，来实现对流量的、不同功能的处理，比如上述的配置文件中的过滤器链，在监听器上添加了 HttpConnectionManager 过滤器，这个过滤器负责解析 HTTP 协议；
- cluster defintios：设置转发到下游的 upsteam server，比如上述配置文件中的 cluster defintios，设置转发到 `xxx` 域名上

**Envoy 提供了 xDS API 标准（x-discovery service），也就是服务发现服务** ，Envoy 通过 xDS API 获取配置信息，然后根据配置信息进行转发，包括几个类型，分别是：EDS（endpoint discovery service），LDS（listener discovery service）和 CDS（cluster discovery service），对应实现节点服务发现，监听器服务发现和集群服务发现。

### 5.6 控制面和数据面交互过程

整个交互过程分为 3 步骤：管理员使用控制面进行策略配置、策略下发数据面、数据面策略生效。

#### 5.6.1 配置 istioctl、kubectl, 调用 k8s api 的过程

![image](https://img2024.cnblogs.com/blog/167509/202409/167509-20240907154621574-234413417.png)

#### 5.6.2 Pilot 通过 xDS 接口进行策略下发

![image](https://img2024.cnblogs.com/blog/167509/202409/167509-20240907154701786-1443496104.png)

#### 5.6.3 流量策略的执行过程

![image](https://img2024.cnblogs.com/blog/167509/202409/167509-20240907154717670-841921566.png)

2. https://www.thebyte.com.cn/ServiceMesh/The-future-of-ServiceMesh.html#footnote-ref2)

## 参考资料

* https://www.cnblogs.com/wzh2010/p/18031098
* https://zhuanlan.zhihu.com/p/61901608
* https://www.cnblogs.com/wzh2010/p/18031103
* https://www.thebyte.com.cn/ServiceMesh/summary.html
* https://link.zhihu.com/?target=https%3A//philcalcado.com/2017/08/03/pattern_service_mesh.html

# [Istio]基本概念

**<font color=blue>Istio 是一个 Service Mesh 形态的，用于服务治理的开放平台。</font>**这里的服务 “治理”不仅限于“微服务”，可以推广到任何服务。只要存在服务或者应用，在它们之间存在访问，也存在对服务与应用的管理，都可以使用到 Istio 。

## Istio 为什么可以实现流量治理

<font color=blue>Istio 中对流量的转发处理都是通过 Envoy 实现的。</font>

Envoy 作为 Sidecar 和每个服务容器部署在同一个 pod 中，Sidecar 在注入到 pod 之后，将原有服务调用从 `源容器 -> 目标容器` 的通信方式改变为 `源容器 -> Sidecar(源端) -> Sidecar(目的端) -> 目的容器`。

## Istio 的架构原理

<font color=blue>Istio 服务网格从逻辑上分为数据平面和控制平面。</font>

- <font color=blue>控制平面管理并配置代理来进行流量路由。</font>
- <font color=blue>数据平面由一组智能代理（Envoy Proxy）组成，被部署为 sidecar。</font>

Istio 架构体系中，流控（Traffic Management）虽然是数据平面的 Envoy Proxy 实施的，但整个架构的核心其实在于控制平面 Pilot。**<font color=blue>Pilot 主要功能就是管理和配置部署在特定 Istio 服务网格中的所有 sidecar 代理实例。</font>**它管理 sidecar 代理之间的路由流量规则，并配置故障恢复功能，如超时、重试和熔断。

![img](https://www.thebyte.com.cn/assets/istio-mesh-arch-cc36d859.png)

这样 Envoy 接管了流入流出用户服务的流量，持有流量策略。并且 Istio 会自动探测 kubernetes 集群的 services 和 endpoints，从而可以获取 services 与 endpoints 之间的关系，Envoy 配置里既有流量策略，又有 endpoints 自然可以实现流量的转发处理。

# [Istio]整体架构

Istio 从逻辑上分为数据平面（Data Plane）和控制平面（Control Plane）。

- **数据平面**：由整个网格内的 Sidecar 代理组成，这些代理以 Sidecar 的形式和应用服务一起部署。这些代理负责协调和控制应用服务之间的所有网络通信。每一个 Sidecar会接管进入和离开服务的流量，并配合控制平面完成流量控制等方面的功能。
- **控制平面**：控制和管理数据平面中的 Sidecar 代理，完成配置分发、服务发现、流量路由、授权鉴权等功能，以达到对数据平面的统一管理。<font color=blue>在 Istio 1.5 版本中，控制平面由原来分散的、独立部署的几个组件整合为一个独立的 istiod，变成了一个单进程、多模块的组织形态。</font>同时，在 Istio 1.5 版本中，基于性能、部署方便考虑，废弃了Mixer，转而将这些功能放到了 Sidecar 中。

下图展示了组成每个平面的不同组件：

![img](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/f17a5a552a674b56b77a20a738ba7cfc~tplv-k3u1fbpfcp-zoom-in-crop-mark:1512:0:0:0.awebp)

## 1、核心组件

下面将简单的介绍一下 `Istio` 架构中几个核心组件的主要功能。

### 1.1 Proxy

Proxy 位于数据平面，即：常说的 Sidecar 代理，与应用服务以 Sidecar 方式部署在同一个 Pod 中。<font color=blue>Proxy 实际上包括 istio-proxy 和 pilot-agent 两部分，它们以两个不同的进程部署在同一个容器 istio-proxy 中。</font>

#### istio-proxy

Istio 的数据平面默认使用 Envoy 的扩展版本作为 Sidecar 代理（即：istio-proxy），istio-proxy 是基于 Envoy 新增了一些扩展，其代码仓库位于 [istio/proxy](https://link.juejin.cn?target=https%3A%2F%2Fgithub.com%2Fistio%2Fproxy)。

注：理论上，Istio 是支持多种 Sidecar 代理，其中 Envoy 作为默认提供的数据平面，如无特殊说明在 Istio 中通常所说的 Envoy 就是 istio-proxy。

Envoy 是用 C++ 开发的高性能代理，用于协调服务网格中所有服务的入站和出站流量，是唯一与数据平面流量交互的组件。主要包括三部分能力：

- 动态服务发现、负载均衡、路由、流量转移。
- 弹性能力：如超时重试、熔断等。
- 调试功能：如故障注入、流量镜像等。

#### polit-agent

pilot-agent，负责管理 istio-proxy 的整个生命周期，具体包括 istio-proxy 准备启动参数和配置文件，负责管理 istio-proxy 的启动过程、运行状态监控以及重启等。其代码仓库位于 [istio/istio/pilot/cmd/pilot-agent](https://link.juejin.cn?target=https%3A%2F%2Fgithub.com%2Fistio%2Fistio%2Fpilot%2Fcmd%2Fpilot-agent)。

<font color=blue>部署上，isito-proxy 不是单独构建镜像，而是和 polit-agent 一起打包构建成一个镜像 istio/proxyv2，poilt-agent 将会以子进程的方式启动 istio-proxy，并监控 istio-proxy 的运行状态。</font>

### 1.2 Istiod

自 Istio 1.5 版本开始，控制平面由原来分散、独立部署的三个组件（Pilot、Citadel、Galley）整合为一个独立的 istiod，变成了一个单进程、多模块的组织形态，极大的降低了原来部署的复杂度。

#### Pilot

<font color=blue>负责 Istio 数据平面的 xDS 配置管理</font>，具体包括：

- **服务发现、配置规则发现**：为 Sidecar 提供服务发现、用于智能路由的流量管理功能（例如，A/B 测试、金丝雀发布等）以及弹性功能（超时、重试、熔断器等）。通过提供通用的流量管理模型和服务发现适配器（`Service Discovery Adapter`），来对接不同平台的适配层。
- **xDS 配置下发**：提供统一的 `xDS API`，供 Sidecar 调用。将路由规则等配置信息转换为 Sidecar 可以识别的信息，并下发给数据平面。

注：这里实际上是指 pilot-discovery，代码仓库位于 [istio/istio/pilot/cmd/pilot-discovery](https://link.juejin.cn?target=https%3A%2F%2Fgithub.com%2Fistio%2Fistio%2Fpilot%2Fcmd%2Fpilot-discovery)

#### Citadel

<font color=blue>负责安全证书的管理和发放，可以实现授权和认证等操作。</font>

Citadel 并不是唯一的证书管理方式，Istio 当前支持 Citadel、Vault 和 Google 等多种证书管理方式，Citadel 是当前默认的证书管理方式。

#### Galley

Galley 是 `Istio 1.1` 版本中新引入的**配置管理**组件，<font color=blue>主要负责配置的验证、提取和处理等功能。其目的是将 Istio 和底层平台（如 Kubernetes）进行解耦。</font>

*在引入 Galley 之前，Istio 控制平面的各个组件需要分别对 Kubernetes 资源进行管理，包括资源的配置验证，监控资源配置变化，并针对配置变更采取相应的处理等。*

## 2、设计目标

几个关键的设计目标形成了 Istio 的架构，这些目标对于使系统能够大规模和高性能地处理服务是至关重要的。

- **<font color=blue>对应用透明性</font>**：从本质上说，对应用透明是 Service Mesh 的特性，一个合格的 Service Mesh 产品都应该具有这一特性，否则也就失去了网格产品的核心竞争力。为此，Istio 自动将自己注入到服务之间的所有网络路径中，做到对应用的透明性。Istio 使用 Sidecar 代理来捕获流量，并在不更改已部署应用程序代码的情况下，自动对网络层进行配置，以实现通过这些代理来路由流量。
- **可扩展性**：Istio 认为，运维和开发人员随着深入使用 Istio 提供的功能，会逐渐涌现更多的需求，主要集中在策略方面。因此，为策略系统提供足够的扩展性，成为了 Istio 的一个主要的设计目标。
- **可移植性**：考虑到现有云生态的多样性，Istio 被设计为可以支持不同的底层平台，也支持本地、虚拟机、云平台等不同的部署环境。不过从目前的情况来看，Istio 和 Kubernetes 还是有着较为紧密的依赖关系，平台无关性、可移植性将是 Istio 最终实现目标。
- **策略一致性**：Istio 使用自己的 API 将策略系统独立出来，而不是集成到 Sidecar 中，从而允许服务根据需要直接与之集成。同时，Istio 在配置方面也注重统一和用户体验的一致性。一个典型的例子是路由规则都统一由虚拟服务来配置，可在网格内、外以及边界的流量控制中复用。

## 3、Istio 架构演进

从 2017 年 5 月发布以来，Istio 经历了四个重要的版本和由此划分的三个发展阶段。在不到三年的产品迭代过程中，出现了两次重大的架构变动。

- 0.1 版本：2017 年 5 月发布。作为第二代 Service Mesh 的开创者，**宣告了 Istio 的诞生**，也燃起了网格市场的硝烟与战火。
- 1.0 版本：发布于 2018 年 7 月，**对外宣传生产环境可用**。从 0.1 到 1.0 版本，开发时间经历了一年多，但持续的发布了多个 0.x 版本，这一阶段处于快速迭代期。
- 1.1 版本：发布于 2019 年 3 月，**号称企业级可用的版本**。一个小的版本号变化居然耗费了半年之久，其主要原因是出现了第一次架构重构，这一阶段算是调整期。
- 1.5 版本：发布于 2020 年 3 月，再次进行**架构的重建，将多组件整合为单体形态的 istiod**。从 1.1 到 1.5 版本的一年中，Istio 开始遵循季节性发布，进入了产品的稳定发展期。

![img](https://xcbeyond.cn/istio-handbook/architecture/istio-architecture-evolution.png)

# [Istio]控制平面

<font color=blue>控制平面是控制和管理数据平面中的 Sidecar 代理，完成配置分发、服务发现、流量路由、授权鉴权等功能，以达到对数据平面的统一管理。</font>

在 Istio 1.5 版本之前，`Mixer` 组件负责遥测统计（`istio-telemetry`）和策略控制（`istio-policy`），其中提供了两个接口 check 和 report，<font color=blue>在每次 Sidecar 代理发送请求时都会去 Mixer 里 check 一次，然后 report 观测信息给 Mixer，Mixer 加工后发送给 `Prometheus`。因此，基于性能、部署方便的考虑，在 Istio 1.5 版本中废弃了 Mixer 组件，转而将这些功能放到了 Sidecar 中。</font>

<font color=blue>在 Istio 1.5 版本中，控制平面由原来分散、独立部署的几个组件整合为一个独立的 istiod，变成了一个单进程、多模块的组织形态。</font>`istiod` 是新版本中最大的变化，以一个单体组件替代了原有的架构，在降低复杂度和维护难度的同时，也让易用性得到提升。需要注意的一点是，原有的多组件并不是被完全移除，而是在重构后以模块的形式整合在一起组成了 `istiod`。

目前，控制平面依旧延续之前组件的功能，但以模块的形式呈现在 `istiod` 中，包括 `Pilot`、`Citadel`、`Galley` 三个模块，本节将会详细介绍它们。

## 1、Pilot

<font color=blue>`Pilot` 是 Istio 中的核心组件，用于管理和配置部署在特定 Istio 服务网格中的所有 Sidecar 代理实例。它管理 Sidecar 代理之间的路由流量规则，并配置故障恢复功能，如超时、重试和熔断等。</font>

### 1.1 Pilot 架构

[![Pilot架构图](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/afd0d5103169457cac7e98323c8b375e~tplv-k3u1fbpfcp-zoom-in-crop-mark:1512:0:0:0.awebp)](https://link.juejin.cn?target=https%3A%2F%2Fxcbeyond.cn%2Fistio-handbook%2Farchitecture%2Fpilot-arch.png)

图 4.3.1：Pilot架构图

根据上图，Pilot 有几个关键模块如下：

#### 抽象模型（Abstract model）

为了实现对不同服务注册中心 （如，`Kubernetes`、`Consul`） 的支持，`Pilot` 需要对不同输入来源的数据有一个统一的存储格式，也就是抽象模型。

抽象模型中定义的关键成员包括 `HostName`（service 名称）、`Ports`（service 端口）、`Address`（service ClusterIP）、`Resolution` （负载均衡策略） 等。

#### 平台适配器 （Platform adapters）

`Pilot` 的实现是基于平台适配器（`Platform adapters`） 的，借助平台适配器 `Pilot` 可以实现服务注册中心数据到抽象模型之间的数据转换。

例如，`Pilot` 中的 `Kubernetes` 适配器通过 `Kubernetes API Server` 获得 `Kubernetes` 中 service 和 Pod 的相关信息，然后翻译为抽象模型提供给 `Pilot` 使用。

通过平台适配器模式，`Pilot` 还可以从 `Consul` 等平台中获取服务信息，还可以开发适配器将其他提供服务发现的组件集成到 `Pilot` 中。

#### xDS API

<font color=blue>`Pilot` 使用了一套源于 Envoy 项目的标准数据平面 API 来将服务信息和流量规则下发到数据面的 Sidecar 中。这套标准数据平面 API，也叫 `xDS`。</font>

Sidecar 通过 `xDS API` 可以动态获取 `Listener` （监听器）、`Route` （路由）、`Cluster`（集群）及 `Endpoint` （集群成员）及 `Secret`（安全）配置：

- <font color=blue>LDS：`Listener` 发现服务。</font>Listener 监听器控制 Sidecar 启动端口的监听（目前只支持 TCP 协议），并配置 `L3/L4` 层过滤器，当网络连接达到后，配置好的网络过滤器堆栈开始处理后续事件。
- <font color=blue>RDS：`Route` 发现服务。</font>于 HTTP 连接管理过滤器动态获取路由配置，路由配置包含 HTTP 头部修改（增加、删除 HTTP 头部键值），virtual hosts （虚拟主机），以及 virtual hosts 定义的各个路由条目。
- <font color=blue>CDS：`Cluster` 发现服务。</font>用于动态获取 Cluster 信息。
- <font color=blue>EDS：`Endpoint` 发现服务。</font>用于动态维护端点信息，端点信息中还包括负载均衡权重、金丝雀状态等，基于这些信息，Sidecar 可以做出智能的负载均衡决策。
- <font color=blue>SDS：`Secret` 发现服务。</font>用于运行时动态获取 TLS 证书。

<font color=blue>通过采用该标准 API， Istio 将控制平面和数据平面进行了解耦，为多种数据平面 Sidecar 实现提供了可能性。</font>例如蚂蚁金服开源的 Golang 版本的 Sidecar MOSN (Modular Observable Smart Network)。

#### 用户 API（User API）

`Pilot` 还定义了一套用户 API， 用户 API 提供了面向业务的高层抽象，可以被运维人员理解和使用。

运维人员使用该 API 定义流量规则并下发到 `Pilot` ，这些规则被 `Pilot` 翻译成数据平面的配置，再通过 `xDS API` 分发到 Sidecar 实例，可以在运行期对微服务的流量进行控制和调整。

通过运用不同的流量规则，可以对网格中微服务进行精细化的流量控制，如按版本分流、断路器、故障注入、灰度发布等。

### 1.2 Pilot 实现

[![Pilot实现](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/c925e9390f40465a90de393339228bb2~tplv-k3u1fbpfcp-zoom-in-crop-mark:1512:0:0:0.awebp)](https://link.juejin.cn?target=https%3A%2F%2Fxcbeyond.cn%2Fistio-handbook%2Farchitecture%2Fpilot-process.png)

图 4.3.2：Pilot实现

图中实线连线表示控制流，虚线连线表示数据流。带 `[pilot]` 的组件表示为 `Pilot` 组件，图中关键的组件如下：

- **Discovery service**：即 `pilot-discovery`，主要功能是从注册中心（如 Kubernetes 或者 Consul ）中获取服务信息，从 `Kubernetes API Server` 中获取流量规则（`Kubernetes CRD Resource`），并将服务信息和流量规则转化为数据平面可以处理的格式，通过标准的数据平面 API 下发到网格中的各个 Sidecar 中。
- **Agent**：即 `pilot-agent` 组件，该进程根据 `Kubernetes API Server` 中的配置信息生成 Envoy 的配置文件，负责启动、监控 Sidecar 进程。
- **Proxy**：既 Sidecar Proxy，是所有服务的流量代理，直接连接 `pilot-discovery` ，间接地从 Kubernetes 等服务注册中心获取集群中微服务的注册情况。
- **Service A/B**：使用了 Istio 的应用，如 Service A/B，进出网络流量会被 Proxy 接管。

其中，`Pilot` 实际上包括 `pilot-discovery` 和 `pilot-agent` 两个组件，分别位于控制平面和数据平面。

#### pilot-discovery

pilot-discovery 位于控制平面，扮演服务发现、控制平面到 Sidecar 之间的桥梁作用。`pilot-discovery` 的主要功能如下：

- 监控服务注册中心（如 Kubernetes）的服务注册情况。在 Kubernetes 环境下，会监控 service、endpoint、pod、node 等资源信息。
- 监控 Istio 控制平面信息变化，在 Kubernetes 环境下，会监控包括 `RouteRule`、 `VirtualService`、`Gateway`、`EgressRule`、`ServiceEntry` 等以 `Kubernetes CRD` 形式存在的 Istio 控制面配置信息。
- 将上述两类信息合并组合为 Sidecar 可以理解的配置信息，并将这些信息以 gRPC 协议提供给 Sidecar。

#### pilot-agent

`pilot-agent` 位于数据平面，是一个本地代理，与 Sidecar 代理部署在一起，负责 Sidecar 服务的整个生命周期。`pilot-agent` 的主要功能如下：

- 生成 Sidecar 的配置。
- 负责 Sidecar 的启动与监控。

## 2、Citadel

`Citadel` 是 Istio 中负责身份认证和证书管理的核心安全组件，1.5 版本之后取消了独立进程，作为一个功能模块被整合在 istiod 中。

### 2.1 Citadel 基本功能

总体来说，Istio 在安全架构方面主要包括以下内容：

- 证书签发机构（`CA`）负责密钥和证书管理。
- API 服务器将安全配置分发给数据平面。
- 客户端、服务端通过代理安全通信。

Istio 的身份标识模型使用一级服务标识来确定请求的来源，它可以灵活的标识终端用户、工作负载等。在平台层面，Istio 可以使用类似于服务名称来标识身份，或直接使用平台提供的服务标识。比如 Kubernetes 的 `ServiceAccount`，`AWS IAM` 用户、角色账户等。

在身份和证书管理方面，Istio 使用 `X.509` 证书，并支持密钥和证书的自动轮换。从 1.1 版本开始，Istio 开始支持安全发现服务器（`SDS`），随着不断的完善和增强，1.5 版本 `SDS` 已经成为默认开启的组件。Citadel 以前有两个功能：将证书以 Secret 的方式挂载到命名空间里；通过 SDS gRPC 接口与 `nodeagent`（已废弃）通信。目前 Citadel 只需要完成与 SDS 相关的工作，其他功能被移动到了 istiod 中。

### 2.2 Citadel 工作原理

`Citadel` 主要包括 `CA` 服务器、`SDS` 服务器、证书密钥控制器等模块，它们的工作原理如下：

#### CA 服务器

`Citadel` 中的 CA 签发机构是一个 gRPC 服务器，启动时会注册两个 gRPC 服务，一个是 CA 服务，用来处理 CSR 请求（`certificate signing request`）；另外一个是证书服务，用来签发证书。CA 首先通过 HandleCSR 接口处理来自客户端的 CSR 请求，对客户端进行身份验证（包括 TLS 认证和 JWT 认证），验证成功后会调用 `CreateCertificate` 进行证书签发。

#### SDS 服务器

SDS， 即安全发现服务（Secret Discovery Service），它是一种在运行时动态获取证书私钥的 API，Envoy 代理通过 SDS 动态获取证书私钥。Istio 中的 SDS 服务器负责证书管理，并实现了安全配置的自动化。相比传统的方式，使用 SDS 主要有以下优点：

- 无需挂载 Secret 卷；
- 动态更新证书，无需重启；
- 可以监听多个证书密钥对。

[![Citadel身份认证流程](https://p3-juejin.byteimg.com/tos-cn-i-k3u1fbpfcp/7e4f86c64e124c13b3a349b8ffe1d555~tplv-k3u1fbpfcp-zoom-in-crop-mark:1512:0:0:0.awebp)](https://link.juejin.cn?target=https%3A%2F%2Fxcbeyond.cn%2Fistio-handbook%2Farchitecture%2Fcitadel-identity-authentication-process.png)

图 4.3.3：Citadel身份认证流程

目前的版本中，SDS 是默认开启的，它的工作流程如下：

- Envoy 通过 SDS API 发送证书和密钥请求；
- istio-agent 作为 Envoy 的代理，创建一个私钥和证书签名请求（CSR），并发送给 istiod；
- CA 机构验证收到的 CSR 并生成证书；
- istio-agent 将私钥和从 istiod 收到的证书通过 SDS API 发送给 Envoy；
- 以上流程周期性执行实现密钥和证书轮换。

#### 证书密钥控制器

证书密钥控制器（`CaSecretController`）监听 `istio.io/key-and-cert` 类型的 `Secret` 资源，它会周期性的检查证书是否过期，并更新证书。

#### 证书轮换

如果没有自动证书轮换功能，当证书过期时，就不得不重启签发，并重启代理。证书轮换解决了这一问题，提高了服务的可用性。Istio 里通过一个轮换器（`Rotator`）自动检查自签名的根证书，并在证书即将过期时进行更新，它本质上是一个协程（`goroutine`）在后台轮询实现的：

- 获取当前证书，解析证书的有效期并获取下一次轮换时间。
- 启动定时器，如果发现证书到达轮换时间，从 CA 获取最新的证书密钥对。
- 更新证书。

## 3、Galley

`Galley` 是不直接向数据平面提供业务能力，而是在控制平面上向其他模块提供支持。

`Galley` 作为负责配置管理的模块，验证配置信息的格式和内容的正确性，并将这些配置信息提供给控制平面的 `Pilot` 使用，这样控制平面的其他模块只用和 `Galley` 交互，从而与底层平台解耦。使用简单的接口和 `Galley` 进行交互，由 `Galley` 负责配置验证、配置变更管理、配置源管理、多平台适配等工作。

# [Envoy]基础概念

## 什么是 Envoy

如果是云原生相关的 devops 工程师应该多多少少都会了解 envoy 这个东西。

在行业大量使用 <font color=blue>微服务架构</font> 和 <font color=blue>云原生解决方案</font> 的今天，一切向“云”靠拢，那么对于云上的 **<font color=blue>流量治理</font>** 与 **<font color=blue>网络调试</font>** 就是一件复杂但必要的事情。

各个微服务可能使用不一致的流量跟踪和记录机制，对于开发人员来说，这方面调试的复杂性大大增加。对于这种类似多对多映射的状况，我们就很难确定问题发生在哪里，以及如何解决。如果你是一个业务开发，而调试网络问题不是你所擅长的话，那就更是如此。

<font color=blue>将网络问题从应用程序堆栈中抽离出来，由另一个组件来处理网络部分，让调试网络问题变得更容易。**这就是 Envoy 所做的事情。**</font>

也就是说，你写的微服务不用再关注一系列网络与流量问题相关的程序逻辑（如接口速率限制，鉴权验证，超时重试等等）这些全部抽离出来，在 envoy 中配置好帮你完成。同时，envoy 会做好流量跟踪记录相关的工作，在多个 envoy 联合工作时，就可以通过一个统一的控制面去收集这些流量信息，治理效果就很显著了。

## 1. Envoy 的两种模式

envoy 是 **云原生时代** 代理服务器应用的一种，和 nginx 类似，但它比 nginx 这种传统代理服务器更胜一筹。

它有两种部署方式，一种是很符合它本质工作的部署方式：**<font color=blue>边缘代理</font>**，这种情况下，envoy 部署用于构建 API 网关，和正常网关的功能差不多。

另一种部署方式就是：**<font color=blue>边车部署（sidecar）</font>**，这种情况下就是前文我们说的，将网络层面的工作从应用程序中“抽离”出来，单独运行。sidecar 方式部署时， envoy 是和应用服务一起部署的。Envoy 和应用程序形成一个原子实体，但仍然是独立的进程。**<font color=blue>应用程序处理业务逻辑，而 Envoy 则处理网络问题。</font>**（最常见的例子在 k8s 里面，就是一个 pod 里面放两个容器，一个跑业务代码，另一个就是envoy，在 pod 内部业务代码容器的流量都会先经过 envoy 容器，再由 envoy 传到 pod 外）

sidecar 部署的方式用来组织当前云原生中的服务治理大杀器——服务网格。

### 1.1 Edge Proxy

Envoy 作为边缘代理（Edge Proxy，也常被称为 Front Proxy 或 API 网关），本质上就是整个内部服务集群对外的 **<font color=blue>统一大门</font>**。

当它被部署在网络边缘时，主要负责接收来自外部（比如互联网用户、外部客户端）的请求，并将其安全、高效地引入到内部网络中。

你可以通过以下 3 个核心点来理解边缘代理的作用：

- **统一的流量入口（Ingress）：** 所有外部进来的流量必须先经过它。它把复杂的外部网络和你的内部微服务架构隔离开来，内部服务不需要直接暴露给公网。
- **集中处理“脏活累活”：** 像 HTTPS 证书卸载（TLS Termination）、身份认证（鉴权）、全局限流、WAF 防护等跨服务通用的安全逻辑，都在这一层统一搞定。后端的微服务可以直接“轻装上阵”，专注核心业务。
- **全局路由与分发：** 它就像个超级交通警察，能够根据请求的 URL 路径、Header、Cookie 等信息，进行复杂的路由匹配，并做负载均衡，把请求精准转发给后端的具体服务。

### 1.2 Edge Proxy 和 Sidecar 的区别

Envoy 作为边缘代理和 Sidecar，其根本区别体现在**网络位置、功能侧重点**以及**资源与生命周期耦合度**这三个层面：

**1. 网络拓扑与流量路径（南北向 vs 东西向）**

- **边缘代理 (Edge Proxy):** 部署在系统架构的网络边界，作为独立的反向代理集群。它专门处理**南北向流量**，即接收来自公网或外部客户端的请求，通过 SNI 或 HTTP Host 等规则进行路由，再转发到内网集群。
- **Sidecar:** 采用与业务进程同机（或同 K8s Pod）共生的下沉式部署。它专注处理集群内部的**东西向流量**（服务 A 调用服务 B）。Sidecar 通常通过 iptables 或 eBPF 等技术在操作系统内核层进行流量劫持（透明代理），完全接管业务容器的进出网络 I/O。

**2. 核心功能侧重点**

- **边缘代理:** 强调**全局入口控制**。计算资源主要消耗在密集的 TLS 卸载（TLS Termination）、全局限流防刷、统一 API 鉴权（如 JWT 验证）上。它负责将复杂的外部网络环境进行清洗和标准化，再引入内网。
- **Sidecar:** 强调**微服务精细化治理**。功能主要聚焦于内部环境的服务发现、L7 智能负载均衡、细粒度的熔断与重试、分布式链路追踪（Header 透传），以及保障内部服务间通信安全的 mTLS 加密。

**3. 生命周期与系统资源开销**

- **边缘代理:** 具有独立的生命周期。它的规模与后端微服务的数量无关，仅根据外部总流量的吞吐量进行独立的水平扩缩容。
- **Sidecar:** 生命周期与具体的业务进程强绑定（1:1 关系）。集群内有上千个服务实例，就会并发运行上千个 Envoy 进程。这种高密度的部署下，大量的控制面配置分发（xDS 解析）和流量劫持，会带来不容忽视的**内存常驻开销和额外的内核网络栈 I/O 延迟**。

简而言之，边缘代理是整个集群对外的标准化网关，而 Sidecar 是实现 Service Mesh（服务网格）架构、解耦业务逻辑与网络控制的核心组件。

## 2. Envoy的高级特性

### 2.1 进程外架构

这里要说的是 Envoy 的 sidecar 部署方式的好处。tetrate 学习资料中对这部分介绍写得比较清晰，我放一下原文：

> Envoy 是一个独立的进程，旨在与每个应用程序一起运行 —— 也就是我们前面提到的 Sidecar 部署模式。集中配置的 Envoy 的集合形成了一个透明的服务网格。
>
> <font color=blue>路由和其他网络功能的责任被推给了 Envoy。</font>应用程序**（进行 DNS 服务发现时）**向一个虚拟地址（localhost）而不是真实地址（如公共 IP 地址或主机名）发送请求，不知道网络拓扑结构。应用程序不再承担路由的责任，因为该任务被委托给一个外部进程。**<font color=blue>（应用程序的流量会先到达 envoy 监听的端口。原理是 envoy 会在 L4 层修改 iptables 规则，进行流量劫持，将出站和入站流量重定向到 Envoy 监听的端口）</font>**
>
> 与其让应用程序管理其网络配置，不如在 Envoy 层面上独立于应用程序管理网络配置。在一个组织中，这可以使应用程序开发人员解放出来，专注于应用程序的业务逻辑。
>
> Envoy 适用于任何编程语言。你可以用 Go、Java、C++ 或其他任何语言编写你的应用程序，而 Envoy 可以在它们之间架起桥梁。Envoy 的行为是相同的，无论应用程序的编程语言或它们运行的操作系统是什么。
>
> Envoy 还可以在整个基础设施中透明地进行部署和升级。这与为每个单独的应用程序部署库升级相比，后者可能是非常痛苦和耗时的。
>
> 进程外架构是有益的，因为它使我们在不同的编程语言 / 应用堆栈中保持一致，我们可以免费获得独立的应用生命周期和所有的 Envoy 网络功能，而不必在每个应用中单独解决这些问题。

“集中配置的 Envoy 的集合形成了一个透明的服务网格”。<font color=blue>服务网格的核心目标是**解决微服务架构中服务间通信的复杂性**，</font>而多语言环境下的互通问题正是其重要应用场景之一。Envoy 很好地契合了这一任务。

### 2.2 L3/L4 过滤器结构

<font color=blue>Envoy 是一个 L3/L4 网络代理，根据 IP 地址和 TCP 或 UDP 端口进行决策。它具有一个**可插拔的过滤器链**（就像 Linux上的管道），可以编写你的过滤器来执行不同的 TCP/UDP 任务。</font>

Envoy 可以通过堆叠所需的过滤器来构建逻辑和行为，形成一个过滤器链。当前已有很多做好了的过滤器，支持诸如原始 TCP 代理、UDP 代理、HTTP 代理、TLS 客户端认证等任务。Envoy 也是可扩展的，我们可以编写我们的过滤器。（特别是当下结合 wasm 技术）

### 2.3 L7 过滤器结构

<font color=blue>Envoy 支持一个额外的 HTTP L7 过滤器层。</font>我们可以在 **HTTP 连接管理子系统（HCM）**中插入 HTTP 过滤器，执行不同的任务，如 cache、速率限制、路由 / 转发等等。

### 2.4 HTTP 相关支持

**HTTP2 支持**

Envoy 同时支持 HTTP/1.1 和 HTTP/2，并且可以作为一个透明的 HTTP/1.1 到 HTTP/2 的双向代理进行操作。这意味着任何 HTTP/1.1 和 HTTP/2 客户端和目标服务器的组合都可以被桥接起来。即使你的传统应用没有通过 HTTP/2 进行通信，如果你把它们部署在 Envoy 代理旁边，它们最终也会通过 HTTP/2 进行通信。

推荐在所有的服务间配置的 Envoy 使用 HTTP/2，以创建一个持久连接的网格，请求和响应可以在上面复用。

**HTTP 路由**

当以 HTTP 模式操作并使用 REST 时，Envoy 支持路由子系统 **HCM**，能够根据 路径、权限、内容类型 和 运行时间值 等规则来路由和重定向请求。

在将 Envoy 作为构建 **API 网关**的前台 / 边缘代理时，这一功能非常有用，在构建服务网格（sidecar 部署模式）时，也可以利用这一功能。

### 2.5 gRPC 支持

Envoy 支持作为 gRPC 请求和响应的路由和负载均衡底层所需的所有 HTTP/2 功能。

> gRPC 是一个开源的远程过程调用（RPC）系统，它使用 HTTP/2 进行传输，并将协议缓冲区作为接口描述语言（IDL），它提供的功能包括认证、双向流和流量控制、阻塞 / 非阻塞绑定，以及取消和超时。

### 2.6 服务发现和动态配置

我们可以使用静态配置文件来配置 Envoy，这些配置文件描述了服务间通信方式，后面我们会有相关学习。

对于静态配置 Envoy 不现实的高级场景，Envoy 支持**动态配置**，在运行时自动重新加载配置。

> 一组名为 **xDS** 的发现服务可以用来通过网络动态配置 Envoy，并为 Envoy 提供关于主机、集群 HTTP 路由、监听套接字和加密信息。

但这个是 sidecar 部署以及服务网格相关的内容了，**简单来说，就是有一个控制平面 动态收集当前微服务集群中的信息，然后将这些信息转换成各种各样的 config.yaml ，再统一下发到各个 sidecar envoy 中。**

### 2.7 健康检查

负载均衡器（LB）有一个特点，那就是只将流量路由到健康和可用的上游服务。

Envoy 支持健康检查子系统（Health Checking），对上游服务集群进行**主动**健康检查。健康检查包括很多种类型，有 HTTP，TCP，gRPC 等，采用类似于定期发送心跳包的方式。

*（但需要后端实现对应心跳包响应逻辑，像 HTTP 主动心跳检查，后端应该实现一个 /health 的接口来响应 200 状态。）*

*（但如果 Envoy 完全依赖 服务网格 EDS 机制（通过监听 Kubernetes 的 Endpoints 状态），并且 Kubernetes 的 readinessProbe 已能准确反映应用的健康状态，那么 Envoy 基本无需额外配置主动健康检查。后面会继续学习。）*

然后，Envoy 使用 服务发现 和 健康检查信息 的组合来确定健康的负载均衡目标。

Envoy 还可以通过 异常点检测子系统（Outlier Detection） 支持**被动**健康检查.

有一个简单的总结表，暂时看不懂也没关系：

| **场景**                      | **是否需要 Envoy 主动检查** | **理由**                                                     |
| :---------------------------- | :-------------------------- | :----------------------------------------------------------- |
| 纯 Kubernetes + EDS           | 否                          | Kubernetes 探针已足够，Envoy 依赖 EDS 状态路由               |
| 需要深度健康检查              | 是                          | 叠加 Envoy 检查以补充 Kubernetes 探针的不足                  |
| 服务网格（如 Istio）          | 否                          | 依赖 Kubernetes 探针 + 被动异常检测，控制平面保障 EDS 数据一致性 |
| 混合环境（K8s + 非 K8s 服务） | 部分需要                    | 非 Kubernetes 服务需通过 Envoy 主动检查                      |

### 2.8 高级负载均衡

Envoy 支持自动重试、断路、全局速率限制（使用外部速率限制服务）、 灰度请求（或流量镜像）、异常点检测和请求对冲。

### 2.10 TLS 终止

> 应用程序和代理的解耦使网格部署模型中所有服务之间的 TLS 终止（双向 TLS）成为可能。

Envoy 可以作为代理，负责处理 TLS 加密连接的建立、解密和验证，而无需后端应用程序直接参与 TLS 握手或加密通信。这样业务代码只需要处理明文流量就好，也无需业务开发在应用中管理证书和密钥。

在服务网格（如 Istio）中，Envoy 作为 Sidecar 代理，**集中管理 TLS 加密和身份认证**。

### 2.11 可观测性

> 为了便于观察，Envoy 会生成日志、指标和追踪。Envoy 目前支持 statsd（和兼容的提供者）作为所有子系统的统计。得益于可扩展性，我们也可以在需要时插入不同的统计提供商。

在服务网格如 Istio 中，Envoy 收集的信息通过经典的 Prometheus + Grafana 的组合进行可视化。

### 2.12 HTTP/3 (Alpha)

Envoy 1.19.0 支持 HTTP/3 的上行和下行，并在 HTTP/1.1、HTTP/2 和 HTTP/3 之间进行双向转义。

## 3. Envoy 架构图

下面给出大致的 Envoy 架构（偏向 Sidecar 部署，其中 控制平面 是服务网格的东西）

具体对过滤器结构的介绍见下一章的【请求流作用模块】。

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1782840453888.5fl9jcm3fq.png)

## 4. Envoy 的构建模块

<font color=blue>在 tetrate 官网上， Envoy 的基本构建模块包括四部分：</font>

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1782840565222.46cyn1scn.png)

但这个构建模块太过抽象，不太好理解，我将更加具体的三层四区抽象放在下面：

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1782840657892.1ow3y413ut.png)

以一个典型的 HTTP 请求为例：

1. 客户端发起 HTTP 请求到 Envoy 监听的端口
2. **监听与接收层**：监听器接受连接并选择过滤器链；网络过滤器识别 HTTP 协议
3. **处理与路由层**：HTTP连接管理器（HCM）解析请求，HTTP 过滤器链处理请求（认证、限流等），路由过滤器确定目标集群
4. **上游服务层**：集群中的负载均衡器选择一个健康的端点，请求被转发到该端点
5. 后端服务处理请求并返回响应，响应沿原路返回客户端
6. **控制与观测区**：整个过程中，配置可能动态更新，请求被记录、统计和追踪

我们主要关注三个层，主要关注静态资源，在后面再介绍学习如何配置动态资源。

那么现在我们先从 **Listener** 开始介绍各个构建模块，思路还是跟着官网的学习路线走，对三层四区的抽象只是更加方便理解。

### 4.1 Listener 监听器

Envoy 暴露的监听器是命名的网络位置，可以是一个 IP 地址和一个端口，也可以是一个 Unix 域套接字路径。<font color=blue>Envoy 通过监听器接收连接和请求。</font>考虑一下下面的 Envoy 配置。

```yaml
 static_resources:
   listeners:
   - name: listener_0
     address:
       socket_address:
         address: 0.0.0.0
         port_value: 10000
     filter_chains: [{}]
```

通过上面的 Envoy 配置，我们在 `0.0.0.0` 地址的 `10000` 端口上声明了一个名为 `listener_0` 的监听器。这意味着 Envoy 正在监听 `0.0.0.0:10000` 的传入请求。

每个监听器都有不同的部分需要配置。然而，唯一需要的设置是地址。上述配置是有效的，你可以用它来运行 Envoy—— 尽管它没有用，因为所有的连接都会被关闭。因为我们让**网络过滤器链** `filter_chains` 字段为空，因为在接收数据包后不需要额外的操作。

为了进入下一个构件（路由），我们需要创建一个或多个`filter_chains`，至少要有一个过滤器。

### 4.2 FilterChain 过滤器链机制

每个通过监听器进来的请求可以流经多个过滤器。我们可以写一个配置，根据传入的请求或连接属性选择不同的过滤器链。过滤器链可以对进入和离开 Envoy 的**请求和响应**进行处理（注意，有来有回，过滤器链不只是处理请求）。

我们先来全面介绍一下 Envoy 中的监听器类别，记得结合三层四区图来看！

Envoy 定义了三类过滤器：监听器过滤器、网络过滤器 和 HTTP 过滤器。**注意，三类过滤器构成了一个层次化的请求处理管道，而不是平行组件。**

- 监听器过滤器

  在收到数据包后立即启动，通常对数据包的头信息进行操作。监听器过滤器包括：

  - 代理监听器过滤器（提取 PROXY 协议头）
  - TLS 检查器监听器过滤器（检查流量是否为 TLS，如果是，则从 TLS 握手中提取数据）。

- 网络过滤器（NetworkFilter）

  通常对数据包的有效载荷进行操作，查看有效载荷并对其进行解析。例如，PostgreSQL 网络过滤器解析数据包的主体，检查数据库操作的种类或其携带的结果。

  - 一个特殊的、内置的网络过滤器被称为 **HTTP 连接管理器**过滤器（HTTP Connection Manager Filter）或 **HCM**。HCM 过滤器能够将原始字节转换为 HTTP 级别的消息。它可以处理访问日志，生成请求 ID，操作头信息，管理路由表，并收集统计数据。我们将在以后的学习中对 HCM 进行更详细的介绍。

- **HTTP 过滤器**，就像我们可以为每个监听器定义多个网络过滤器（其中一个是 HCM）一样，Envoy 也支持在 HCM 过滤器中定义多个 HTTP 级过滤器。我们可以在名为 `http_filters` 的字段下定义这些 HTTP 过滤器。

总的来说是下面这样：

1. Listener 有自己的监听器过滤器，除此之外，一个 Listener 包含一个或多个 FilterChain 。
2. 一个FilterChain 包含一个或多个 NetworkFilter 。
3. HCM 是其中一种 NetworkFilter ，HTTPFilter 组成了 HCM 中的 HTTPFilterChain
4. RouterFilter 是其中一种 HTTPFilter，放在 HTTPFilterChain 的最后执行。

> 注意，我们在 4.1 中的配置文件中，只定义了一个监听器(`listener_0`) 和 一个空的过滤器链 `filter_chains: [{}]`，但这是网络过滤器链(Network Filter Chain)的部分，而不是监听器过滤器。监听器过滤器需要在配置中使用 `listener_filters` 字段定义。

### 4.3 Router

<font color=blue>HTTP 过滤器链中的最后一个过滤器必须是路由器过滤器（`envoy.filters.HTTP.router`）。</font>路由器过滤器负责执行路由任务。这最终把我们带到了第二个构件 —— **路由**。

#### 域名匹配

我们在 **HCM 过滤器**的 `route_config` 字段下定义路由配置。在路由配置中，我们可以通过查看元数据（URI、Header 等）来匹配传入的请求，并在此基础上，定义流量的发送位置。

路由配置中的顶级元素是**虚拟主机** `virtual_hosts` 。每个虚拟主机元素都有一个属于自己的`name`，在发布统计数据时使用（不用于路由），还有一组被路由到它的域名`domain`。

让我们考虑下面的路由配置和域名的集合。

```yaml
 route_config:
   name: my_route_config
   virtual_hosts:
   - name: cnblogs_hosts
     domains: ["cnblogs.com"]
     routes:
     ...
   - name: test_hosts
     domains: ["test.cnblogs.com", "qa.cnblogs.com"]
     routes:
     ...
```

如果传入请求的目的地是 `cnblogs.com`（即 HTTP 请求中 `Host/Authority` 标头被设置为其中一个值），则该请求会走向`cnblogs_hosts` 虚拟主机中定义的路由。

同样，如果 `Host/Authority` 标头包含 `test.cnblogs.com` 或 `qa.cnblogs.com`，则该请求会走向`test_hosts` 虚拟主机下的路由。这样我们就可以用一个监听器（`0.0.0.0:10000`）来处理多个顶级域名。

如果你在数组中指定多个域，搜索顺序如下：

1. 精确的域名（例如：`cnblogs.com`）。
2. 后缀域名通配符（如 `*.cnblogs.com）`。
3. 前缀域名通配符（例如：`cnblogs.*`）。
4. 匹配任何域的特殊通配符（`*`）。

#### 路由匹配

在 Envoy 匹配域名后，是时候处理所选虚拟主机中的 `routes` 字段了。我们用该字段，指定如何匹配一个请求，以及接下来如何处理该请求（例如，重定向、转发、重写、发送直接响应等）。

我们来看看一个例子。

```yaml
 static_resources:
   listeners:
   - name: listener_0
     address:
       socket_address:
         address: 0.0.0.0
         port_value: 10000
     filter_chains:
     - filters:
       - name: envoy.filters.network.http_connection_manager
         typed_config:
           "@type": type.googleapis.com/envoy.extensions.filters.network.http_connection_manager.v3.HttpConnectionManager
           stat_prefix: hello_world_service
           http_filters:
           - name: envoy.filters.http.router
           	 typed_config:
             	"@type": type.googleapis.com/envoy.extensions.filters.http.router.v3.Router
           route_config:
             name: my_first_route
             virtual_hosts:
             - name: direct_response_service
               domains: ["*"]
               routes:
               - match:
                   prefix: "/"
                 direct_response:
                   status: 200
                   body:
                     inline_string: "envoy yay"
```

配置的顶部部分与我们之前看到的一样。我们已经添加了 HCM 过滤器、统计前缀（`hello_world_service`）、单个 HTTP 过滤器（路由器）和路由配置。

我们把 `route_config` 的部分拿出来：

```yaml
route_config:
	name: my_first_route
    virtual_hosts:
    - name: direct_response_service
      domains: ["*"]
      routes:
      - match:
    		prefix: "/"
        direct_response:
    		status: 200
    		body:
    			inline_string: "envoy yay"
```

在虚拟主机内，我们要匹配任何域名（通配符 `*` 指定）。在 `routes` 下，我们匹配前缀（`/`），然后我们可以发送一个响应。

当涉及到匹配请求时，我们有多种选择。

| 路由匹配          | 描述                                     | 示例                                                         |
| ----------------- | ---------------------------------------- | ------------------------------------------------------------ |
| `prefix`          | 前缀必须与`:path` 头的开头相符。         | `/hello` 与 `hello.com/hello`、`hello.com/helloworld` 和 `hello.com/hello/v1` 匹配。 |
| `path`            | 路径必须与`:path` 头完全匹配。           | `/hello`匹配 `hello.com/hello`，但不匹配 `hello.com/helloworld`或 `hello.com/hello/v1` |
| `safe_regex`      | 所提供的正则表达式必须与`:path` 头匹配。 | `/\{3}` 匹配任何以 `/` 开头的三位数。例如，与 `hello.com/123` 匹配，但不能匹配 `hello.com/hello` 或 `hello.com/54321。` |
| `connect_matcher` | 匹配器只匹配 CONNECT 请求。              |                                                              |

一旦 Envoy 将请求与路由相匹配，我们就可以对其进行路由、重定向或返回一个直接响应。在这个例子中，我们通过 `direct_response` 配置字段使用**直接响应**。

> 在 tetrate 的教学过程中，他们使用了一个名为 [func-e](https://func-e.io/) 的命令行工具。**func-e 允许我们选择和使用不同的 Envoy 版本。**
>
> 安装 func-e CLI 的方式见func-e官网。
>
> 当然，也可以直接下载 Envoy 的 bin 文件来跑。
>
> 更加折腾的方式是自己下载 Envoy 源码进行编译。但是编译的环境要求非常高（建议用编译环境镜像做这个事情），并且编译时间会非常非常非常长……

现在把上述配置保存到 `envoy-direct-response.yaml` 中后，我们可以用这个配置运行 Envoy。

```sh
func-e run -c envoy-direct-response.yaml
```

如果是自己拉下来的二进制 envoy，则：

```sh
envoy-1.27.0-linux-x86_64 -c conf.yaml
```

一旦 Envoy 启动，我们就可以向 `localhost:10000` 发送一个请求，以获得我们配置的直接响应。

```powershell
$ curl localhost:10000
envoy yay
```

同样，如果我们添加一个不同的主机头（例如 `-H "Host: hello.com"`）将得到相同的响应，因为 `hello.com` 主机与虚拟主机中定义的域相匹配。

### 4.4 Cluster 和 Endpoint

在大多数情况下，从配置中直接发送响应是一个很好的功能，但我们会有一组端点或主机，我们将流量路由到这些端点或主机。在 Envoy 中做到这一点的方法是通过定义**集群**。

集群（Cluster）是**一组**接受流量的上游类似主机。这可以是你的服务所监听的主机或 IP 地址的**列表**（类似 k8s 中 Service 与 EndpointSlice 的关系）

例如，假设我们有一个 hello world 服务监听 `127.0.0.0:8000` 。然后，我们可以用一个单一的端点创建一个集群，像这样：

```yaml
static_resources:
  clusters:
    - name: hello_world_service
      connect_timeout: 0.25s
      type: STRICT_DNS
      lb_policy: ROUND_ROBIN
      hosts:
        - socket_address:
            address: 127.0.0.1
            port_value: 8000
```

**集群的定义与监听器的定义在同一级别**，使用 `clusters` 字段。我们在路由配置中引用集群时，以及在导出统计数据时，都会使用集群这个字段部分。该集群名称在所有集群中必须是唯一的。

我们还可以定义集群内部端点的**负载均衡**：

```yaml
 clusters:
 - name: hello_world_service
   load_assignment:
     cluster_name: hello_world_service
     endpoints:
     - lb_endpoints:
       - endpoint:
           address:
             socket_address:
               address: 127.0.0.1
               port_value: 8000
```

在 `load_assignment` 字段下，我们可以定义要进行负载均衡的端点列表，以及负载均衡策略设置。每个 `lb_endpoints` 包含一个或多个端点，对于 endpoint 部分见 4.5 介绍。

Envoy 支持多种负载均衡算法（round-robin、Maglev、least-request、random），这些算法是通过一系列设定共同配置的，这些相关设定有：静态引导配置、DNS、动态 xDS（CDS 和 EDS 服务）以及 主动 / 被动健康检查。如果我们没有通过 `lb_policy` 字段明确地设置负载均衡算法，它默认为 round-robin。

*（当然这里的 Envoy负载均衡只是一个了解，后面会有单独的笔记深入学习。）*

我们可以先来了解一下 endpoint 部分负载均衡支持的一些特性。通常情况下，负载均衡器对所有端点一视同仁，但集群定义允许在端点内建立一个层次结构。

例如，端点可以有一个 **权重（weight）** 属性，这将指示负载均衡器与其他端点相比，向这些端点发送更多 / 更少的流量。

另一种层次结构类型是基于**地域（locality）**属性，通常用于定义故障转移架构，在容灾处理和平常的业务优化都很有用。这种层次结构允许我们定义地理上比较接近的 "首选" 端点，以及在 "首选" 端点变得不健康的情况下应该使用的 "备份" 端点。

我们还可以在 Cluster 中配置以下可选功能：

- 主动健康检查（`health_checks`）
- 断路器 (`circuit_breakers`)
- 异常点检测（`outlier_detection`）
- 在处理上游的 HTTP 请求时有额外的协议选项
- 一组可选的网络过滤器，应用于所有出站连接等

和监听器的地址一样，端点地址可以是一个套接字地址，也可以是一个 Unix 域套接字。

### 4.5 示例结合 yaml

让我们看看这些配置是如何结合起来的。

```yaml
static_resources:
  listeners:
  - name: listener_0
    address:
    socket_address:
    address: 0.0.0.0
    port_value: 10000
  filter_chains:
  - filters:
    - name: envoy.filters.network.http_connection_manager
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.http_connection_manager.v3.HttpConnectionManager
      stat_prefix: hello_world_service
      http_filters:
      - name: envoy.filters.http.router
      route_config:
        name: my_first_route
        virtual_hosts:
        - name: direct_response_service
          domains: ["*"]
          routes:
          - match:
              prefix: "/"
            route:
              cluster: hello_world_service
  clusters:
  - name: hello_world_service
    connect_timeout: 5s
    load_assignment:
      cluster_name: hello_world_service
      endpoints:
      - lb_endpoints:
        - endpoint:
            address:
              socket_address:
                address: 127.0.0.1
                port_value: 8000
```

与前面的yaml不同，我们添加了集群配置，没有使用 `direct_response`，而是使用 `routes` 字段并指定集群名称。

我们在本地跑一个 golang 的 hello-world 服务来验证这个配置，监听 8000 端口：

```golang
package main
 
import (
    "fmt"
    "net/http"
)
 
func helloWorld(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "Hello, World!")
}
 
func main() {
    http.HandleFunc("/", helloWorld)
    fmt.Println("Starting server at port 8000")
    if err := http.ListenAndServe(":8000", nil); err != nil {
        fmt.Println(err)
    }
}
```

然后我们可以向 `127.0.0.1:8000` 发送一个请求，以检查我们是否得到 "Hello World" 的响应。

接下来，让我们把上述保存了的 Envoy 配置送到 Envoy 启动。

```r
envoy-1.27.0-linux-x86_64 -c envoy-cluster.yaml
```

当 Envoy 代理启动时，向 `0.0.0.0:10000` 发送一个请求，让 Envoy 代理请求到 hello world 端点。

```sh
$ curl -v 0.0.0.0:10000
*   Trying 0.0.0.0:10000...
* Connected to 0.0.0.0 (0.0.0.0) port 10000
* using HTTP/1.x
> GET / HTTP/1.1
> Host: 0.0.0.0:10000
> User-Agent: curl/8.12.1
> Accept: */*
> 
* Request completely sent off
< HTTP/1.1 200 OK
< date: Tue, 01 Apr 2025 12:23:00 GMT
< content-length: 13
< content-type: text/plain; charset=utf-8
< x-envoy-upstream-service-time: 0
< server: envoy
< 
* Connection #0 to host 0.0.0.0 left intact
Hello, World!
```

从冗长的输出中，我们会注意到由 Envoy 代理设置的响应头 `x-envoy-upstream-service-time` 和 `server: envoy`。

## 5. 一些学习需要用到的工具

- 轻量级压测工具 hey：https://github.com/rakyll/hey
- docker 和 python3
- 关于 GLIBC：我的envoy基于1.27版本，需要 2.29及以上的 GLIBC，像腾讯云的centos服务器一般只有 2.28，这个要注意。
- 前期了解envoy特性为目标的话，尽量不要浪费时间在手动编译上。直接去下载二进制文件，我的1.27版本： https://github.com/envoyproxy/envoy/releases/tag/v1.27.0 ，当前最新是1.33
