# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅 P3 캐시 정책 논의를 이어가자.
먼저 docs/P3-CACHE-POLICY-REVIEW.md를 읽어줘.
버전별 불변 자산 묶음 권장안은 제안이며 아직 미합의·미구현이야.
검토 문서 commit·push는 정책 구현 승인이 아니야. 사용자 합의 후에만 구현해줘.
repo /home/jy/work/dali/dali-web, origin https://github.com/dalihub/dali-web.git.
2026-11-06 캐시 정책 검토 시작 HEAD/실제 origin main:
f81276e13314ae68170b09767782abd229a4a958, 최초 clean.
이번 인계는 캐시 정책 검토·문서 갱신이며 정책 코드 변경은 없어.
이 문서가 포함된 커밋과 최종 채팅 SHA가 다음 기준이야.
과거 미커밋/commit·push 없음은 검사 당시 기록으로 취급해줘.
실제 HEAD/status/원격부터 다시 확인하고 후속 사용자 커밋 상태를 우선해줘.
기존 변경·실패 로그·증거를 보존해줘.

상위 AGENTS.md, STATE·PLAN·HANDOFF·BUILDING,
P3-SW-RECOVERY-RESULTS와 evidence/p3-sw-recovery/README,
수정 전 P3-SW-LIFECYCLE-RESULTS·evidence/p3-sw-lifecycle/README,
P3-EVENT-PTHREAD-RESULTS·evidence/p3-event-pthread/README,
P1-EXECUTION-CONTRACT·PLAYGROUND-COMPILATION,
P3-GPU-MEMORY-RESULTS·evidence/p3-gpu-memory/README·P3-INITIAL-RESULTS,
HANDOFF가 지정한 필수 문서를 읽어줘.

수정 전 unregister 뒤 재방문 실패로 D4 중단했지만 사용자가 복구 제안을
승인(수정 고고)했고 구현/검증 완료했어. 이전 복구 승인 대기는 해소됐어.
- tests/browser/production.js/html: SW URL별 retry flag, 성공 후 제거,
  자동 reload 최대1회, 등록/ready/controller 총5초 timeout,
  늦은 완료 후 자동 reload 차단, 오류와 다시 시도 버튼.
- tests/browser/check-sw-lifecycle.py 최종 logs/p3/sw-recovery-003 exit0.
  기존3.1.24 build/p3/event-pthread-cmake-001 기본 E 산출물 사용.
- 최초/재방문/update/update503 실패 후/unregister 재방문 및
  등록503·제어없음·6.5초 지연제어·격리헤더없음 실패→명시적 재시도.
  총9회 실제2프레임·GL error0·ready→stopped.
- 실패4종에서 UI 표시·runtime 미시작, 자동 reload0/0/0/1 관측.
  지연제어는 timeout 후 실제 controller 도착해도 자동 진행 없음.
- 잘못된 SW는 harness가 제거/응답복원 후 버튼을 눌렀어.
  자동 SW repair·배포 복구 구현으로 확대하지 마.
- 직접 헤더 E 회귀 sw-recovery-headers-001 exit0.
- 중간001 WebDriver 전역 const 접근 오류,002 HTTP cache 때문에
  navigation 계수 누락. 최종003 HTML에만 no-store로 계수 확인.
  운영/Pages 캐시 정책 채택이 아니며 중간 실패 증거 보존.
- 기존748개·CMake 입력376개 hash drift0, 브라우저/서버 종료.
  공통/native·SDK·lock·SW 원본 변경 없음. 배포 없음.

bootstrap 복구 단위 완료, P3 전체 진행 중. 다음은 사용자 지시에 따라
운영 캐시 계약(network-only 또는 버전별 자산 묶음)·실패/업데이트 검증.
HTTP cache/자산 버전 혼용/rollback·전체 offline/복구·다중 탭·실행 중
update·브라우저 재시작·Chromium·실제 Pages는 미검증.
실제 Pages 검증/배포는 별도 승인. P1 D4 중단 조건 유지.

기본 E 유지, 독립 C 이벤트 pthread 조기 검사 완료 사실 유지.
Core/TLS·이벤트 소유권·실제 렌더·초기화/callback 무한 루프 중 main
응답성·소속2개 pthread 종료·Canvas 제거·새 Module 재생성은 기존
결과가 유효해. 강제 종료는 정상 소멸이나 GPU 즉시 회수·장기 누수
없음의 증거가 아니야. P4 시간/입력/resize·숨김/재개·반복/실패 수명,
추가 pthread·context loss/OOM·장기 누수는 남아 있어.

원본 Emception+Emscripten3.1.24 서버 없는 브라우저 컴파일 유지.
기존6.0.13 SDK/lock·원본/P0/과거 산출물·증거 보존, 새 build/log,
EMCC_CORES=6·최대-j6. SDK/공통/public 계약·전체 플랫폼·합의 방향
변경 필요시 중단·보고. upstream 갱신·운영 서버·배포·다른 관리형 호스팅
전환 금지. loopback 파일 제공 허용. 결과·특이 사항·미검증·다음 작업을
STATE·PLAN·JOURNAL·HANDOFF·NEXT-CONTEXT에 갱신해줘.
작업 단위가 완료돼 다음 컨텍스트로 넘어갈 수 있으면 문서 갱신 후
commit·origin/main push하고 실제 원격 SHA/clean을 확인한 다음
복사 가능한 다음 프롬프트를 제공해줘. HANDOFF의 완료 인계 규칙을 따라줘.
```
