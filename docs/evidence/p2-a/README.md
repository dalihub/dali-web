# P2-a 검토용 증거

2026-10-23. [결과](../../P2-a-RESULTS.md), [manifest/재현](../../P2-a-MANIFEST.md), [P2-b 계획](../../P2-b-HANDOFF.md).

- [commands.json](commands.json): 최종 diagnostic-003 각 버전/configure/build/compile/nm/link의 argv·cwd·개별 exitCode. `${DALI_WEB_ROOT}`, `${DALI_WEB_EMSDK}`, `${DALI_WEB_HOST_TOOLS}`는 공개용 경로 정규화 표기다.
- [versions.txt](versions.txt): 도구 실제 출력.
- [result.json](result.json): 컴파일 개수, 두 strict 링크의 전체 미해결 목록과 완료/미검증 판정.
- [link-webgl2.txt](link-webgl2.txt), [link-full-es3.txt](link-full-es3.txt): linker 오류 원문(경로 정규화). 임시 response 파일은 실행 후 보존되지 않으며 argv는 commands.json에 있다.
- [nm-boundaries.json](nm-boundaries.json): 주요 경계 object 7개의 전체 undefined 목록과 Core defined-symbol 대조. undefined 목록 자체는 최종 미해결 목록이 아니다.
- [source-hashes.json](source-hashes.json): 선택 생산 소스·생성 템플릿·harness·CMake의 SHA256.
- [validation.json](validation.json): 원본/P0·과거 증거·lock·기존 Core 산출물 보존과 변경 범위 검사.

전체 로그는 ignored `logs/p2-a/`에 있다. 최초 configure 실패(001), 초기 harness 결과(002), 참조를 보강한 최종 결과(003)를 덮어쓰지 않았다. 001/002의 명령·개별 exit code는 각 로컬 commands.json, remote 최초 DNS 실패/호스트 성공과 보존 스냅샷은 run-001에 있다. Core 전체 compile_commands·nm·대용량 build 로그는 공개 사본에 포함하지 않는다.
