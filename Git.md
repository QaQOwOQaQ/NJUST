# Git

## 简介

**Git** 是一个免费且开源的 **<font color=blue>分布式版本控制系统</font>**（Distributed Version Control System），最初由 Linux 的创始人 Linus Torvalds 在 2005 年创建。

简单来说，它的主要作用是记录项目文件内容的变化，帮助你管理代码的不同版本，并支持多人高效地协作开发。你可以把它想象成代码的“时光机”和“记账本”。

### 为什么我们需要 Git？

- **<font color=ee11ff>版本控制（快照）：</font>** 每次你保存并“提交”（Commit）代码，Git 都会记录下当前的状态的快照。如果新写的代码出现了严重的 Bug，你可以随时将代码回退到之前正常工作的任何一个版本，就像游戏里的“读档”一样。
- **<font color=ee11ff>分布式架构：</font>** 与传统的集中式版本控制（如 SVN）不同，Git 是分布式的。这意味着每个开发者的电脑上都有一份完整的代码库（包含所有的历史记录）。即使没有网络，你依然可以在本地进行代码提交、查看历史等操作。
- **<font color=ee11ff>强大的分支管理：</font>** Git 极其鼓励使用分支（Branch）。你可以为一个新功能或修复一个 Bug 创建一个独立的分支，在这个分支上的修改不会影响主线代码（通常叫 `main` 或 `master`）。确认没问题后，再将其“合并”（Merge）回主线。
- **<font color=ee11ff>团队协作：</font>** 当多人同时修改同一个项目时，Git 可以追踪每个人做了什么修改。如果两个人修改了同一个文件的同一行，Git 会提示“冲突”（Conflict），并让开发者手动确认保留哪一部分，从而避免代码被意外覆盖。

### Git 的核心工作流

理解 Git 的关键在于理解它管理文件的四个核心区域：

1. **<font color=ee11ff>工作区（Working Directory）：</font>**你在电脑上实际看到、编辑和修改的文件目录。
2. **<font color=ee11ff>暂存区（Staging Area / Index）：</font>**当你修改了文件后，你需要先将这些改动添加到暂存区（通过 `git add` 命令），准备进行下一步的提交。
3. **<font color=ee11ff>本地仓库（Local Repository）：</font>** 存储在你电脑本地的版本库。当你把暂存区的内容提交后（通过 `git commit` 命令），这些修改就永久记录在了你的本地历史中。
4. **<font color=ee11ff>远程仓库（Remote Repository）：</font>** 托管在网络上的版本库（例如 GitHub、GitLab 或 Gitee）。你可以将本地仓库的代码推送到远程仓库（通过 `git push` 命令）与他人共享，或者从远程仓库拉取最新的代码（通过 `git pull` 命令）。

### Git 会导致仓库体积膨胀吗？

直觉上，如果每次提交都保存一份项目的完整拷贝，仓库的体积肯定会迅速膨胀，最终导致硬盘空间不足。

但实际上，Git 的底层设计非常精妙，它通过以下几个核心机制，极其高效地控制了空间占用：

- **<font color=ee11ff>文件级去重 (内容寻址)：</font>** Git 不是按照“版本”来机械复制文件的，而是按**<font color=24b3aa>文件内容</font>**来存储的。每次提交时，Git 会根据文件内容计算出一个 <font color=24b3aa>哈希值</font>。如果某个文件在两次提交之间**<font color=24b3aa>没有发生任何修改</font>**，Git 底层只会存储该文件一次，<font color=24b3aa>新的版本仅仅保存一个指向旧文件的“轻量级指针”。</font>
- **<font color=ee11ff>增量压缩 (Delta Compression)：</font>** 尽管 Git 概念上保存的是项目的完整“快照”，但在磁盘管理层面，它会<font color=24b3aa>定期生成包文件（Packfiles）。</font>在这个过程中，Git 会寻找历史版本中相似的文件，并且**<font color=24b3aa>只保存它们之间的差异（Delta）</font>**。例如，如果你在一个 10MB 的日志文件中增加了一行，打包后 Git 底层只会存储这增加的几十个字节，而不是保存两个完整的 10MB 文件。
- **<font color=ee11ff>底层文本压缩：</font>** Git 存储的所有对象（包括代码内容、提交记录、目录树信息）在写入磁盘前，都会使用 zlib 算法进行高强度的无损压缩。纯文本代码的压缩率是非常惊人的，通常能将体积缩小到原来的几分之一。
- **<font color=ee11ff>自动垃圾回收 (`git gc`)：</font>** 当仓库累积了太多零散的修改记录时，Git 会在后台静默运行垃圾回收机制（或者你也可以手动执行 `git gc`）。这个命令会清理那些不再被任何分支引用的废弃文件（比如被删除的分支产生的游离提交），并强制执行上述的打包压缩，释放大量空间。

<font color=blue>尽管 Git 对 **纯文本代码** 的管理堪称完美，但它确实存在一个弱点：**频繁修改的大型二进制文件**。</font>

对于像设计文件（`.psd`）、视频（`.mp4`）、大型数据库文件或编译产物（`.dll`, `.jar`）这类二进制文件，Git 很难进行高效的增量差异比对。这意味着你每次修改并提交这些大文件，Git 可能都会硬生生地保存一个全新的完整副本，这才是导致 Git 仓库体积爆炸的真正罪魁祸首。

**应对方案：** 对于包含大量此类文件的项目，业界通用的解决方案是使用 **<font color=blue>Git LFS (Large File Storage)</font>**。<font color=24b3aa>它会将大文件的实际内容存放在专门的远程服务器上，而在本地 Git 仓库中只保留一个只有几十字节大小的“文本指针”。</font>

### Github 常用工作流

将 remote 的代码拉取到本地（`git clone`），然后新建一个 my-feature 分支进行个人开发（`git checkout -b my-feature`），不要污染主分支，开发过程中可以通过 git diff 查看当前分支所做的修改。

在此期间，如果主分支 remote 端产生了更新，可以切换到主分支然后拉去 remote 最新的更新（`git pull origin main`），随后切换到 my-feature 分支，执行命令 `git rebase main`，意思是在最新的 main 基础上进行我们的修改。

开发完成后，通过 add-commit-push 三连，推送到 remote，如果想将我们的更新合并到主分支，可以发起一个 PR（Pull Request），remote 在合并时，可以执行 Squash and merge，将我们的包含多个 commit 的更新合并为一个 commit，简化 commit 记录。

> 首先从远程仓库拉取代码到本地：
>
> ```bash
> git clone <repo-url>
> cd <repo-name>
> ```
>
> 然后基于主分支创建自己的功能分支进行开发，避免直接污染主分支：
>
> ```bash
> git checkout -b my-feature
> ```
>
> 开发过程中，可以随时查看当前分支相对于上次提交的修改：
>
> ```bash
> git diff
> ```
>
> 如果开发期间远程主分支有了新的更新，通常做法是先切换回主分支并拉取最新代码：
>
> ```bash
> git checkout main
> git pull origin main
> ```
>
> 再切回自己的开发分支，并把自己的修改“重新放到”最新的主分支之上：
>
> ```bash
> git checkout my-feature
> git rebase main
> ```
>
> 这样做的好处是让提交历史更线性，减少无意义的 merge commit。
>
> 开发完成后，执行 add / commit / push：
>
> ```bash
> git add .
> git commit -m "feat: add my feature"
> git push origin my-feature
> ```
>
> 然后在 GitHub 上发起 PR（Pull Request），请求将 `my-feature` 合并到 `main`。
>
> 如果你的分支上有多个开发过程中的 commit，仓库维护者在合并 PR 时可以选择：
>
> **Squash and merge**
>
> 这样会把多个 commit 压缩成一个 commit 再合并进主分支，便于保持主分支提交记录简洁。
>
> ------
>
> ## 两个小提醒
>
> 1. `rebase` 过程中如果有冲突，需要手动解决后执行：
>
>    ```bash
>    git add .
>    git rebase --continue
>    ```
>
> 2. 如果你的分支已经 push 过，rebase 后再 push 往往需要：
>
>    ```bash
>    git push --force-with-lease origin my-feature
>    ```
>
>    这个比 `--force` 更安全一些。

## 常用命令

### 1. 基础配置与初始化

开始写代码前的准备工作，主要设置个人信息和初始化仓库。

| **命令**                             | **作用说明**                                      |
| ------------------------------------ | ------------------------------------------------- |
| `git config --global user.name xxx`  | 设置全局用户名（记录在 `~/.gitconfig`）           |
| `git config --global user.email xxx` | 设置全局邮箱（记录在 `~/.gitconfig`）             |
| `git init`                           | 将当前目录初始化为 Git 仓库（生成 `.git` 文件夹） |

### 2. 日常开发与查看

| **命令**                                              | **作用说明**                                                 |
| ----------------------------------------------------- | ------------------------------------------------------------ |
| `git add XX`                                          | 将文件添加到暂存区                                           |
| `git commit -m "备注"`                                | 将暂存区内容提交到当前分支                                   |
| `git status`                                          | 查看当前仓库的文件状态                                       |
| **<font color=blue>`git diff XX`</font>**             | 查看文件相<font color=blue>**对于暂存区**</font>修改了哪些内容 |
| `git log` / `git log --pretty=oneline`                | 查看当前分支的历史版本 / 简化的单行显示                      |
| **<font color=blue>`git reflog`</font>**              | 查看 HEAD 指针的移动历史（找回被回滚的版本全靠它）           |
| **<font color=blue>`git stash`</font>**               | 将工作区和暂存区未提交的修改**存入栈中**（临时保存）         |
| **<font color=blue>`git stash pop` / `apply`</font>** | 恢复栈顶修改，`pop` 会同时删除栈顶元素，`apply` 不删除       |
| **<font color=blue>`git stash drop` / `list`</font>** | 删除栈顶存储的修改 / 查看栈中所有元素                        |

### 3. 分支管理

| **命令**                      | **作用说明**                                         |
| ----------------------------- | ---------------------------------------------------- |
| `git branch`                  | 查看所有本地分支，并标出当前所在分支                 |
| `git branch branch_name`      | 仅创建新分支                                         |
| `git branch -d branch_name`   | 删除本地分支                                         |
| `git checkout branch_name`    | 切换到指定分支                                       |
| `git checkout -b branch_name` | **<font color=blue>创建并直接切换</font>**到该新分支 |
| `git switch branch_name`      | 切换到指定分支                                       |
| `git merge branch_name`       | **<font color=blue>将目标分支合并到当前分支</font>** |

### 4. 撤销与代码回滚

| **命令**                                                     | **作用说明**                                            |
| ------------------------------------------------------------ | ------------------------------------------------------- |
| **<font color=blue>`git checkout -- XX` 或 `git restore XX`</font>** | 撤销工作区的修改（还没 add 的文件还原）                 |
| **<font color=blue>`git restore --staged XX`</font>**        | 将文件从暂存区撤回（已经 add 但没 commit）              |
| **<font color=blue>`git rm --cached XX`</font>**             | 停止追踪该文件，将其从索引中删掉，但不删本地实体文件    |
| **<font color=blue>`git reset --hard HEAD^`</font>**         | 代码彻底回滚到上一个版本（`HEAD^^` 往上两次，以此类推） |
| **<font color=blue>`git reset --hard HEAD~100`</font>**      | 往上回滚 100 个版本                                     |
| **<font color=blue>`git reset --hard 版本号`</font>**        | 精确回滚或跳转到某一个指定的 commit 版本                |

### 5. 远程交互

| **命令**                                  | **作用说明**                                                 |
| ----------------------------------------- | ------------------------------------------------------------ |
| `git clone git@...XXX.git`                | 将远程仓库下载到本地当前目录                                 |
| `git remote add origin git@...`           | 将本地代码库关联到指定的远程仓库                             |
| `git pull`                                | 拉取远程当前分支并与本地合并                                 |
| `git push origin branch_name`             | 将本地分支推送到远程仓库                                     |
| `git push -u origin branch_name`          | <font color=blue>`-u` 推送并建立追踪关系（第一次推送新分支时使用）</font> |
| `git push -d origin branch_name`          | 删除远程仓库的分支                                           |
| `git checkout -t origin/branch_name`      | 将远程分支拉取到本地，并自动创建切换对应的本地分支           |
| `git branch --set-upstream-to=origin/A B` | 手动将远程的 A 分支与本地的 B 分支建立对应关系               |

git remote -v ?



## FAQ

### 2. 暂存区

通过暂存区我们可以只提交部分修改。

另外，暂存区可以暂存部分修改，而不是整个文件，我们可以使用 `git add -p` 命令来交互式的选择哪些部分的修改需要添加到暂存区。

如果误将某些文件添加到了暂存区，可以使用 `git reset HEAD <file>` 命令取消这些文件的暂存状态。

### 3. 如何查看当前分支的详细信息

```bash
# 查看当前分支的详细信息
git branch -v

# 查看当前分支的跟踪信息
git branch -vv

# 查看当前分支的所有详细信息
git status
```

### 4. `git reset` 的作用是什么

<font color=blur>当我们使用 `git reset` 进行回滚时，**是否丢失本地修改，取决于 `git reset` 使用的模式：**</font>

| 命令                               | HEAD/提交位置 | 暂存区 | 工作区本地修改 |
| ---------------------------------- | ------------- | ------ | -------------- |
| `git reset --soft HEAD~1`          | 回退          | 保留   | 保留           |
| `git reset --mixed HEAD~1`（默认） | 回退          | 重置   | 保留           |
| `git reset --hard HEAD~1`          | 回退          | 重置   | **丢失**       |

`git reset` 的核心作用是**“时光倒流”**，也就是用来**撤销之前的代码提交（commit）**。它通过把当前的指针（HEAD）往回拨，让你回到过去的某个代码状态。

<font color=blur>在日常开发中，它主要有 3 种模式，代表了“后悔”的不同程度：</font>

1. **`--soft`（温柔回退）** 只撤销 commit 动作，你写的代码修改原封不动地保留着，并且还在暂存区（绿字状态）。 *适用场景：*刚点完提交，突然发现漏加了一个文件或者注释写错了，想补进去重新提交。
2. **`--mixed`（中度回退，默认模式）** 撤销 commit，同时把代码移出暂存区（变成红字状态），但你写的代码修改本身依然保留在电脑里。 *适用场景：*你想保留刚才写的代码，但是觉得之前的提交太乱了，想重新梳理一下，分批次慢慢再提交。
3. **`--hard`（彻底重来）** 这是最无情的模式。不仅撤销提交，还会**把暂存区和本地写好的代码全部清空**，你的代码会完完全全变成指定历史版本原本的样子。 *适用场景：*你发现最近写的代码方向完全错了，全是垃圾，想彻底清空重写。（**注意：用之前千万确认好，未提交的代码会被直接删掉找不回来。**）

### 5. 如何查看文件的更新历史？

```bash
# 查看项目的提交历史
git log

# 查看指定文件的提交历史
git log -- [file_name]

# 查看特定提交的详细信息
git show [commit_sha]

# 比较两个不同版本之间的文件差异
git diff [commit_sha1] [commit_sha2] -- [file_name]
```

### 6. `git tag` 命令有什么作用？

`git tag` 的核心作用是给代码历史打**“书签”**或**“里程碑”**。

它用来给某次特定的代码提交（commit）起一个好记的名字。最常见的场景就是用来**标记版本发布**（比如 `v1.0.0`、`v2.1`）。比起去记一长串无规律的 commit ID（比如 `8a2fb3e...`），使用标签名显然直观得多。

在日常开发中，它的核心用法可以概括为以下 3 点：

- **打标签（标记版本）：** 当项目完成了一个阶段性目标准备上线时，你可以用 `git tag -a v1.0 -m "发布1.0正式版"` 给当前代码状态建立一个“附注标签”。它会记录是谁打的标签、打的时间以及备注信息，非常适合用于正式发布。
- **查看与回退里程碑：** 直接输入 `git tag` 就能列出所有打过标记的版本号。如果线上版本出了严重 bug，你可以随时用 `git checkout v1.0` 快速切换回当时发布的旧版本来排查问题。
- **同步给团队：** 标签默认只存在你的本地电脑里。如果要和整个团队共享这个版本标记，需要额外把它推送到远程仓库，比如使用 `git push origin v1.0`（推送单个）或 `git push origin --tags`（推送所有）。

简单来说，如果 `git commit` 是记录代码走过的每一步细碎脚印，那么 `git tag` 就是在这条路上立起的一块块醒目的**里程碑指示牌**。

```bash
## 一、给当前 commit 打 tag
# 1. 把修改好的代码放进暂存区
git add index.html

# 2. 生成一次正式的提交记录（必须先有它作为载体）
git commit -m "完成首页核心功能开发"

# 3. 给刚刚的那次提交，打上“v1.0”的醒目标签
git tag -a v1.0 -m "发布 1.0 正式版"

# 4. （可选）把这个版本标签推送到服务器，让团队其他人也能看到
git push origin v1.0

## 二、给历史 commit 打 tag
# 1. 找到历史提交的 hash
git log --online

# 2. 如果 hash 是 9fceb02，打标记
git tag -a v0.9 -m "补充发布0.9测试版" 9fceb02
```

### 7. git 中的文件有哪些状态？

在 Git 中，文件主要有 4 种核心状态，它们对应着代码从“刚写完”到“正式入档”的整个流转过程：

1. **<font color=blue>未跟踪 (Untracked)：</font>** 这是新文件的状态。你刚在项目里新建了一个文件，Git 还没把纳入管理视线，对它“不闻不问”。 *操作：使用 `git add` 会让它变成“已暂存”状态。*
2. **<font color=blue>已修改 (Modified)：</font>** Git 已经认识这个文件了（以前提交过），并且你刚刚修改了它的内容，但**还没有**把这次修改做记录（终端里通常显示为红字）。 *操作：使用 `git add` 把修改放入暂存区。*
3. **<font color=blue>已暂存 (Staged)：</font>** 你觉得代码改得差不多了，给它打了个勾，放到一个待提交的清单里，准备放进下一次的版本记录中（终端里通常显示为绿字）。 *操作：使用 `git commit` 正式把清单里的内容存入仓库。*
4. **<font color=blue>已提交 (Committed)：</font>** 文件的修改已经安全地永久保存在你电脑的 Git 仓库里了。此时如果不做新修改，文件就会静静待在这里（也叫 Unmodified 状态）。

### 8. Pull Requese（PR）

Pull Request（简称 PR）简单来说，就是一次**“代码合并申请”**。

在团队开发中，它的核心作用可以概括为以下 3 步：

- **提出申请：** 你在自己的分支上完成了开发，想把这些代码正式合并到项目的主干（Main 分支）中，于是你发起一个 PR，等于告诉团队：“我的代码写好了，请查看。”
- **代码审查 (Review)：** 团队成员会收到通知，打开 PR 就能清晰地看到你增删改了哪些代码。大家可以在这互相讨论、指出错误、要求修改，或者确认没问题并点赞（Approve）。
- **正式合并 (Merge)：** 只要审查通过，项目负责人就会点击合并按钮，你的代码就真正成为了主项目的一部分。

**用大白话打个比方：** 假设你们团队在合写一本书（主分支）。你把书拿走复印了一份，在复印件上加了一段精彩的内容（你的分支）。写完后，你把这段新内容递交给主编，说：“这是我写的新段落，请审核一下，没问题的话就加进原稿里吧。”

这个**“递交修改草稿并请求审核入库”**的动作，就是提一个 PR。

### 9. 如何将一个分支的更改合并到另一个分支？

#### `git merge`

把一个分支（比如开发用的 `feature` 分支）的更改合并到另一个分支（比如主干 `main`），核心思路非常简单：**先站过去，再拉过来**。

日常操作通常只有以下 3 步：

**1. 切换到“接收代码”的目标分支** 你必须先站在你想把代码放进去的那个分支上。比如要合并进 `main` 分支，就先切过去：

```bash
git checkout main
```

**2. 执行合并命令** 告诉 Git，把带有新更改的分支（`feature`）拉过来，和现在的分支融为一体：

```bash
git merge feature
```

*如果一切顺利，Git 会自动完成合并，这就搞定了。*

**3. 解决冲突（遇到报错时才需要）** 如果两个分支刚好修改了同一个文件的同一行，Git 不敢擅自决定听谁的，就会提示“合并冲突（Conflict）”。 这时你只需要：

- 手动打开报错的文件，删掉 Git 自动生成的 `<<<<<<<` 等乱码标记，保留你真正想要的代码。
- 重新把改好的文件放入暂存区并提交，告诉 Git 冲突修好了：

```bash
git add .
git commit -m "解决合并冲突"
```

----

这是我们通过 `git merge` 将 my_feature 分支上的修改合并到 master 后，master 分支上的 log 信息：

```
commit 4ed778bd29102be974d88c55a6daa5eb85cff201 (HEAD -> master)
Merge: 066709c e4d68bb
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:06:50 2026 +0800

    [merge] modify readme.md

commit 066709cebc306dbec713b35632f5be5d058b28a2
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:06:08 2026 +0800

    [master] modify readme.md

commit e4d68bba68e3e41dc785c6971d6d7cd8cfcbfe49 (my_feature)
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:05:26 2026 +0800

    [my_feature] modify readme.md

commit 36cfde090f3c7878787967ea7f96c191bc042d33
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:03:55 2026 +0800

    add readme.md
```

my_feature 分支上的 log 信息：

```
commit e4d68bba68e3e41dc785c6971d6d7cd8cfcbfe49 (HEAD -> my_feature)
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:05:26 2026 +0800

    [my_feature] modify readme.md

commit 36cfde090f3c7878787967ea7f96c191bc042d33
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:03:55 2026 +0800

    add readme.md
```

---

#### `git rebase`

`git rebase`（变基）的核心作用是**“移花接木”**或**“重新排队”**，主要用来**保持代码历史记录的绝对直线**。

它和刚才提到的 `git merge` 目的一样，都是把两个分支的代码合在一起，但两者的处理逻辑完全不同：

- **`merge`（岔路汇合）：** 尊重历史。它会保留你拉出分支、独立开发、最后汇合的完整轨迹，并生成一个新的“合并提交”。代码历史看起来是一张**交错的网状图**。
- **`rebase`（重新排队）：** 改写历史。它会把你在分支上写的代码“连根拔起”，然后按顺序重新“贴”到主干分支的最顶端。代码历史里没有岔路，也没有合并记录，看起来就像大家都在同一条线上排队写代码一样，是一条**完美的直线**。

**操作示例（<font color=blue>将 main 的最新进展同步到你的分支</font>）：**

```bash
git checkout feature  # 1. 站在你自己的分支上
git rebase main       # 2. 告诉Git：把我的新代码，重新排到 main 分支的最新节点后面
```

*(如果中间遇到冲突，解决冲突后使用 `git add .` 然后执行 `git rebase --continue` 继续排队。)*

----

还是同样的例子，现在我们将 master 上的信息通过 `git rebase` 合并到 my_feature。

合并前 master：

```
commit be6f1a51d9c7ea2840b495522130b4ecaebaf20e (HEAD -> master)
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:14:32 2026 +0800

    [master:add] modify readme.md

commit 165b52c3fd8d05a9e39cf1aa20e3c1ca23637552
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:12:55 2026 +0800

    [master:add] add readme.md
```

合并前 my_feature:

```
commit 7272433865b3aba8a58bb421ac9b1a11af943078 (HEAD -> my_feature)
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:13:35 2026 +0800

    [my_feature:add] 1st modify readme.md

commit 165b52c3fd8d05a9e39cf1aa20e3c1ca23637552
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:12:55 2026 +0800

    [master:add] add readme.md
```

my_feature rebase 后：

```
commit ad8ef1b4f2d7746169950364da2b9c2847fd7314 (HEAD -> my_feature)
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:13:35 2026 +0800

    [my_feature:add] 1st modify readme.md, and merge main's modify

commit be6f1a51d9c7ea2840b495522130b4ecaebaf20e (master)
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:14:32 2026 +0800

    [master:add] modify readme.md

commit 165b52c3fd8d05a9e39cf1aa20e3c1ca23637552
Author: xjy <2407217576@qq.com>
Date:   Fri Apr 24 20:12:55 2026 +0800

    [master:add] add readme.md
```

### 10. git 怎么合并多个 commit 

在 Git 里把多个零碎的 commit 合并成一个整洁的大 commit，最常用也最强大的方法是 **<font color=blue>交互式变基（Interactive Rebase）</font>**。大白话就是让 Git 把最近的几条历史记录摊开，你手动把它们揉成一团。

核心操作就这三步：

**第一步：圈定要合并的范围** 在终端输入 <font color=blue>`git rebase -i HEAD~3`</font>（这里的 `3` 代表你想合并最近的 3 个 commit，数字可以根据你的实际情况改）。

**第二步：把状态改成“合并” (Squash)** 敲完命令后，系统会弹出一个文本编辑器，里面按顺序（从旧到新）列出了你最近的这几个 commit。

- 把第一行的 commit 前面保留默认的 `pick`（把它当作主心骨）。
- 把下面想揉进去的 commit 前面的 `pick` 全都改成 `squash`（或者只写 `s`，意思是把这条合并到上一条里）。 改完后保存并关闭这个文件。

**第三步：重写最终的提交留言** 上一步保存后，Git 会立刻再弹出一个窗口，把那几个 commit 原本的留言都堆在一起。你可以把乱七八糟的旧留言全删了，只写一句总结性的新留言。再次保存退出，合并就搞定了！

**一个保命提醒：** 如果这些 commit **已经推送到远端仓库**了，你合并完必须用 `git push -f`（强制推送）才能覆盖云端记录。自己拉的分支随便搞，但如果是大家共用的主分支，千万别这么干，会把同事的代码搞乱。

> 假设你在开发一个“登录按钮”，中间修修补补提交了 3 次：
>
> - `commit A`: 新增登录按钮（hash `1111111`）
> - `commit B`: 调整按钮颜色（hash `2222222`）
> - `commit C`: 修复按钮文字拼写错误（hash `3333333`）
>
> 要是直接推上去，同事看你的提交记录会觉得很啰嗦。现在我们要把这 3 个揉成一个干净的提交。
>
> **1. 圈定范围** 在终端输入：`git rebase -i HEAD~3`。
>
> **2. 揉成一团（改动作）** 弹出的文本编辑器里，本来长这样（越靠上的是越老的代码）：
>
> ```
> pick 1111111 新增登录按钮
> pick 2222222 调整按钮颜色
> pick 3333333 修复按钮文字拼写错误
> ```
>
> 保留最上面的 `pick` 作为主心骨，把后面的 `pick` 全改成 `s`（squash 的简写，意思是“融合进上一条”），变成这样并保存退出：
>
> ```
> pick 1111111 新增登录按钮
> s 2222222 调整按钮颜色
> s 3333333 修复按钮文字拼写错误
> ```
>
> **3. 总结一句话** 紧接着系统会自动弹出第二个窗口，里面堆着你原来的 3 句旧留言。你直接把它们全删掉，清清爽爽地写上最终的总结：
>
> ```
> 完成登录功能UI开发
> ```
>
> 保存退出。搞定！
>
> 现在你再用 `git log` 看，那 3 个零碎的脚印就消失了，变成了一个干净漂亮的大 commit。

### 11. git 中，要提交更新到主线，主线提示冲突，如何解决?

遇到 Git 冲突别慌，这只是因为你和别人同时改了同一处代码，Git 不知道该听谁的。只需简单三步就能搞定：

**1. 拉取最新代码并定位冲突** 如果你是在推送（push）时被拒，说明远端有新代码。先运行 `git pull origin main`（假设主线叫 main）。此时终端会红字提示哪些文件存在冲突（conflict）。

**2. 手动修改冲突文件（最关键一步）** 打开那些提示冲突的文件，你会看到 Git 自动插入了类似这样的标记：

```bash
<<<<<<< HEAD
你本地修改的代码...
=======
别人在主线上提交的最新代码...
>>>>>>> 远端版本号
```

**怎么做：** 决定好这两段代码要留哪个、删哪个，或者怎么融合。然后把那些多余的 `<<<<<<<`、`=======`、`>>>>>>>` 符号全删掉，保存文件。

**3. 重新提交并推送到远端** 冲突内容改好保存后，就可以告诉 Git 你解决完了。依次执行：

- `git add .` （把改好的文件标记为已解决）
- `git commit -m "解决合并冲突"` （提交这次修复）
- `git push origin main` （顺利推送到主线）

---

也可以先 `git fetch` 将更新拉取到本地的 `origin/main`，然后再通过 `git diff main origin/main` 查看改动，然后再通过 `git merge main origin/main` 进行合并。

### 12. git exclude

git exclude 指的是 Git 仓库内部的 `.git/info/exclude` 文件。

简单来说，它的作用和 `.gitignore` 完全一致（语法也一样），唯一的区别是**作用范围和私密性**：

- <font color=blue>**完全私有，不会被提交**：`.git` 目录本身不受版本控制，所以写在 `exclude` 里的规则只对你本地当前仓库生效，绝不会被 `git push` 到远端，团队其他成员完全无感知。</font>
- **规则仅限当前仓库**：它只影响当前这一个 local repo，不像全局配置（`~/.gitignore_global`）那样作用于你电脑上的所有项目。

**最常用的场景**：

- 存放你个人的本地调试工具、临时脚本、测试数据（如 `debug.log`、`temp-test.py`）。
- <font color=blue>针对自己本地环境的特定配置或 IDE 辅助文件，而你不希望把这些“个人偏好”污染到项目的共享 `.gitignore` 中。</font>

**使用方法**： 直接用编辑器打开项目下的 `.git/info/exclude`，像写 `.gitignore` 一样把需要忽略的路径或通配符加进去即可：

```
# 在 .git/info/exclude 中添加
my_private_notes.md
local_debug_data/
```

> <font color=blue>**注意**：和 `.gitignore` 一样，如果某个文件已经被 Git 跟踪（tracked），直接写进 `exclude` 是无效的，需要先 `git rm --cached <file>` 把它移出版本控制。</font>

### 13. `git commit --amend`

`git commit --amend` 用来修改最近一次提交，而不是创建一条新的提交记录。

典型的使用场景：

- 上次提交的 commit message 写错了。
- 上次提交漏掉了一个文件。
- 上次提交的代码有个小错误，需要一起提交。

#### 场景 1：修改最近一次提交信息

假设执行：

```bash
git commit -m "fix: login bug"
```

发现提交信息不准确，应该写成 `fix: user login bug`。

可以执行：

```bash
git commit --amend -m "fix: user login bug"
```

Git 会创建一个新的提交来替换当前分支的最新提交。

#### 场景 2：上次提交漏了文件

假设执行：

```bash
git add main.cpp
git commit -m "feat: add login"
```

但是忘记添加 `login.h` 文件。

可以执行：

```bash
git add login.h
git commit --amend --no-edit
```

这里：

- `--amend`：修改上一次提交。
- `--no-edit`：保留原来的 commit message。

最终表现为一次提交，而不是两次。

#### --amend 的底层原理

Git 的提交对象包含：

- 当前提交对应的 Tree 对象。
- 父提交的哈希值。
- 作者信息和提交者信息。
- 提交时间等元数据。
- Commit Message。

Git 的 commit 是不可变对象。

因此，`--amend` 不是直接修改旧 commit，而是创建一个新的 commit 对象，将当前分支指向新 commit。

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1792657637361.7f7941.webp)

#### 不建议修改已经 push 的提交

假设本地和远程都有提交 C，你在本地执行：

```bash
git commit --amend
```

本地会变成 C'，远程仍然是 C。

这时候直接：

```bash
git push
```

通常会因为不是 fast-forward 而被拒绝。

你可能需要：

```bash
git push --force-with-lease
```

但要注意，这会重写远程分支历史，可能影响其他协作者。

原则：尽量只对尚未推送的提交使用 `--amend`。

#### Git reset、revert、amend 的区别

| 命令                 | 核心作用                                   | 是否改写提交历史     |
| -------------------- | ------------------------------------------ | -------------------- |
| `git commit --amend` | 替换最近一次提交                           | 是                   |
| `git reset`          | 移动 HEAD/分支指针，可同时重置索引和工作区 | 是（移动当前分支时） |
| `git revert`         | 创建新提交来反向撤销指定提交               | 否                   |

### 14. `git revert`

`git revert` 用于撤销某次提交（commit）所引入的修改，**同时保留 Git 历史记录。**

**它不会删除原来的提交，而是创建一个新的提交，用来抵消之前的修改。**

#### 1. 基本用法

```bash
git revert <commit-id>
```

例如：

```bash
git log --oneline
```

输出：

```
a3f92d1 添加登录功能
b7c84e2 修复页面布局
c91a563 初始化项目
```

执行：

```bash
git revert a3f92d1
```

Git 会创建一个新提交，撤销 `a3f92d1` 中引入的修改。

#### 2. 常用命令

| 命令                                 | 作用                                   |
| ------------------------------------ | -------------------------------------- |
| `git revert HEAD`                    | 撤销最近一次提交                       |
| `git revert HEAD~1`                  | 撤销倒数第二次提交                     |
| `git revert <commit-id>`             | 撤销指定提交                           |
| `git revert --no-commit <commit-id>` | 撤销修改但暂不提交                     |
| `git revert --no-edit HEAD`          | 撤销最近一次提交，不打开提交信息编辑器 |
| `git revert --abort`                 | 发生冲突时，终止正在进行的 revert      |

#### 3. `git revert` 与 `git reset` 的区别

假设 Git 历史为：

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1792657940793.b5ad30.webp)

如何选择？

- 已经 push 到远程仓库： 通常使用 `git revert`，安全地保留团队共享的提交历史。
- 尚未 push，想彻底取消提交： 可以考虑 `git reset`。
- 只想撤销提交，但保留文件修改： 使用 `git reset --soft HEAD~1`。

#### 4. 撤销连续多个提交

例如撤销最近三个提交：

```bash
git revert --no-commit HEAD~3..HEAD
git commit -m "Revert last three commits"
```

这会尝试逆向应用三个提交的修改，然后统一创建一个撤销提交。

注意： `git revert` 可能产生代码冲突，尤其是需要撤销较早的提交时。遇到冲突，需要先解决冲突，然后执行：

```bash
git add .
git revert --continue
```

## 参考资料

* [Learn Git Branch：可视化的闯关学习](https://learngitbranching.js.org/?demo=&locale=zh_CN)
* [**Git常用命令参考手册**](https://github.com/liuzi6612/git-manual)