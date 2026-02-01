# 贡献指南

感谢你的贡献！为保持仓库整洁和安全，请遵循以下规则。

## 分支模型
- `main`：受保护的稳定分支，仅通过 PR 合并。
- `develop`：日常集成分支，功能完成后合并到此，再择机合并 `main`。
- `feature/*`：新功能开发。
- `bugfix/*`：缺陷修复。
- `hotfix/*`：紧急修复，完成后同时合并 `main` 与 `develop`。

## 提交流程
1. 从 `develop` 拉取最新代码：`git checkout develop && git pull`.
2. 基于对应前缀创建分支，例如：`git checkout -b feature/xyz`.
3. 提交信息采用约定式格式：`<type>: <summary>`，常见 type：`feat`、`fix`、`chore`、`docs`、`refactor`、`test`.
4. 推送前请运行本地构建或单测（如适用）。

## Pull Request 规范
- 标题：与提交格式一致或更详尽描述。
- 内容：简述变更、测试情况、风险与回滚方案。
- 检查列表：勾选模板中的项目，确保未遗漏。
- 需要至少 1 名 CODEOWNERS 评审通过后再合并。

## 代码风格
- 行尾统一 LF（`.gitattributes` 已规范），Windows 脚本保留 CRLF。
- 使用 `.editorconfig` 的缩进和结尾空行要求。
- C/C++ 代码保持 4 空格缩进，适当添加说明性注释。

## 安全
- 不要提交密钥、证书或账号信息；如误提交请立即联系维护者并吊销凭据。
- 若发现安全漏洞，请按 `SECURITY.md` 的私密报告方式处理。
