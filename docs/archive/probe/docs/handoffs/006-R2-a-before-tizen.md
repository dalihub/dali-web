# 다음 context 인수인계: R2-a

갱신일: 2026-10-22. R1-b 및 상위 R1 완료. 다음 작업은 R2-a 실험 코드·빌드 경로 이관과 Core 빌드·Node 검증입니다.

## 먼저 읽을 것

1. `/home/jy/work/dali/AGENTS.md`.
2. 기존 `webassembly-probe/README.md`, `docs/STATE.md`, 이 HANDOFF, `docs/PLAN.md`의 R2-a 부분.
3. R1-b 결과 (로컬 보존 자료: `webassembly-probe/docs/handoffs/R1-b-RESULTS.md`; [위치 안내](../../../../MIGRATION.md)), R1-a 결정 (로컬 보존 자료: `webassembly-probe/docs/handoffs/R1-a-UPSTREAMS.md`; [위치 안내](../../../../MIGRATION.md)).
4. 새 repo의 README·upstreams.lock.json·docs/UPSTREAMS.md.

## 현재 기준

- 새 소스 repo: `/home/jy/work/dali/dali-web`.
- 원격: https://github.com/dalihub/dali-web, branch `main`.
- R1-b 최종 commit: `888ff1423ca205bbaf65c493ee8d5771257f9a6a`. 원격 push 및 SHA 일치 확인.
- Core·Adaptor·UI는 R1-a 고정 SHA의 전체 tracked tree를 squash import. 일반 clone에서 소스·lock·라이선스 검증 PASS. 세 원본 URL·full SHA·tree는 lock 참고.
- R1-b 시작 시 원격 refs와 로컬 경로가 없었으며 기존 변경을 덮어쓰지 않았습니다. 원본 UI 사용자 변경 2개, P0 복제본·로그·SDK 보존.
- 현재 기록 기준: `/home/jy/work/dali/webassembly-probe/docs`. R2-b 완료 전 전환하지 않습니다. 새 repo의 `docs/r1-a/`는 역사적 스냅샷입니다.

## 다음 첫 행동과 범위

1. 새 repo의 HEAD·branch·status·origin 및 기존 P0 복제본 상태를 재확인합니다. 시작점이 달라졌으면 기존 변경을 보존합니다.
2. 기존 `build-core.sh`·Core probe·CMake와 관련 스크립트의 실제 의존 파일을 읽어 이관 범위를 확정합니다. `src/dali-*`를 새 `libs/dali-*` 경로로 연결하고 상위 cmake·scripts·tests 경계를 사용합니다.
3. SDK는 기존 로컬 설치를 재사용할 수 있으나 사용자별 절대 경로를 새 repo 필수 조건으로 만들지 않습니다. 도구 버전과 탐색·오버라이드 절차를 기록합니다. SDK·빌드 결과·프로필·대용량 로그·생성 헤더를 source와 섞지 않습니다.
4. 새 경로에서 Core Wasm 빌드와 Node 애니메이션 검사(중간 50, 최종 100)를 재현합니다. 최대 `-j6`. 빌드·Node 로그는 별도 R2-a 실행 디렉터리에 남기고 기존 P0 로그를 덮어쓰지 않습니다.
5. 이관 파일·경로 변경·명령·종료 코드·새 commit 여부와 한계를 기록하고 R2-a 완료 후 멈춥니다. Firefox·GL 탐색 결과 비교와 기록 기준 전환은 R2-b에 남깁니다.

## 검증 한계와 반복하지 않을 조사

Core·Adaptor 선택 tree는 P0와 같지만 생성 헤더·SDK·빌드 결과는 R2에서 다시 검증해야 합니다. UI 결합 빌드는 P6 대상입니다. 모의 그래픽 애니메이션 결과는 실제 픽셀 렌더링 증거가 아닙니다. 생산 렌더러·스레드·Windows 지원을 완료로 보지 않습니다.

R1-b에서는 SDK 설치·코드 이관·빌드·Node·브라우저 실행을 하지 않았습니다. 원본 선정과 subtree 재import를 반복할 필요는 없습니다. `/tmp/dali-r1b-verify-clone`은 검증용이며 작업 repo로 쓰지 않습니다. R1-b가 남긴 실행 프로세스는 없습니다.

이전 인수인계는 005-R1-b (로컬 보존 자료: `webassembly-probe/docs/handoffs/handoffs/005-R1-b.md`; [위치 안내](../../../../MIGRATION.md)), 증거는 `../logs/r1-b/`, 다음 요청문은 NEXT-CONTEXT (로컬 보존 자료: `webassembly-probe/docs/handoffs/NEXT-CONTEXT.md`; [위치 안내](../../../../MIGRATION.md))에 있습니다.
