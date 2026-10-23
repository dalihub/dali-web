# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅을 이어가자.
P2-b 웹 메모리부터 기본 WebGL2 생산 strict 링크 구현까지 진행해줘.
P1 D1~D4·P2-a 완료, P2-b source/context/sync 중간 구현 상태야.
전체 P2-b와 상위 P2는 미완료야.

작업 repo: /home/jy/work/dali/dali-web
현재 기록: dali-web/docs
과거 증거·SDK: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web
P2-a commit: a68e3f00f0c7752561137c179500b004b930285e
source/context commit: 7b6ee23ff36b3658fc957e1607de2d21963796c1
sync checkpoint는 실제 HEAD/git log로 확인해줘. 원격 push는 하지 않았어.

상위 AGENTS.md와 README, docs/STATE.md, HANDOFF.md, PLAN.md,
P1-EXECUTION-CONTRACT.md, P2-a-MANIFEST.md, P2-a-RESULTS.md,
P2-b-HANDOFF.md, P2-b-RESULTS.md, P2-b-SYNC-RESULTS.md,
evidence/p2-a/README.md, evidence/p2-b/README.md,
evidence/p2-b-sync/README.md, NEXT-CONTEXT.md,
upstreams.lock.json, toolchain.lock.json, BUILDING.md, UPSTREAMS.md,
MIGRATION.md를 먼저 읽어줘. 실제 HEAD/status/원격과 기존 변경을 확인하고 lock을 보존해줘.

현재 구현:
- GlesGraphicsController rename, native context/RTTI·native rendering EGL 경계 분리.
- Web 단일 surface/resource cache identity. 실제 host는 아직 없어.
- SyncPool Shared/Aging 구현을 native-gles-sync-pool.cpp / web-gles-sync-pool.cpp로 분리.
  concrete EGL storage는 cpp 내부 Impl, native FD/cast/age/wait 정책은 native에 보존.
- texture dependency policy와 Controller sync factory도 native/web 파일 선택.
  내부 resource factory helper 추출로 기존 allocator/recycle/deleter 재사용.
  Core·GraphicsInterface·GraphicsSyncAbstraction 계약 변경 없음.
- WebFence: glFenceSync → flush(제출만) → zero-timeout client poll.
  pending/complete/failed/cancelled 구분. frame age·CPU flush를 완료로 취급하지 않아.
  GL owner poll/delete, 다른 스레드는 게시 상태 조회/해제 요청, owner collection이 삭제.
  Web shared fence는 같은 frame에서도 재사용하지 않음(이후 제출을 포함하지 못하기 때문).
  native FD/EGL/cross-context GPU wait는 명시 거부. 단일 context 명령 순서와 GPU 완료는 구분.
- HasFailure()는 GL owner thread 수명 동안 실패를 latch해.
  실제 host가 owner에서 읽어 비동기 failure/stop으로 전달해야 해.
  실패 후 새 Worker/실행 인스턴스 사용. fence shutdown 후 context 폐기 순서 유지.
- DALI_GLES_ENVIRONMENT 기본은 NATIVE_EGL. WEB production closure 미완성으로 configure 거부.
  cmake/p2-b는 Web sync를 선택하지만 native Memory2/3·wrapper·binary cache가 남는 부분 진단.
  정식 source closure와 native generated source 전체 정리는 아직 남았어.

최종 검증:
- logs/p2-b/sync-web-003: pthread Core 333단계, 34개 compile/nm 각각 exit 0.
  기본 WebGL2/FULL_ES3 strict 각각 exit 1, 미해결 10/6개, map 없음.
  NativeFence 3개가 제거됐고 cache utility 5 + GL 5/1개가 남아.
- fence-unit-002: 실제 WebFence 코드에 GL 응답 주입, compile/link·Node 각각 0.
  pending 유지, 두 signalled 결과, creation failure, WAIT_FAILED, 다른 thread 해제/owner 삭제,
  double release, 교체, 종료 cancellation, context loss 검사.
  테스트 전용 --wrap은 생산에 사용하지 않음. 실제 GPU/browser 성공 증거가 아니야.
- sync-sources-001: GLES/VULKAN/DYNAMIC 기존 source 보존 + context 2/sync 2 추출, Vulkan 동일.
- sync-native-002: native configure 3개 host compiler 부재로 각각 1.
  native context/rendering/sync/sync-pool/egl-graphics 5개 교차 compile/nm 각각 0.
  Emscripten 결과이며 native build/install/runtime 성공은 아니야.
- 과거 context-web-003의 32개·13/9개, P2-a의 30개·16/12개 증거/산출물은 그대로 보존.
- cmake/p2-a는 rename/native 추출을 반영. 정확한 과거 재현은 해당 commit 기준.

확정 설계:
1. 공통 GLES + Native/EGL·Web 환경 구현 파일 분리, 빌드 시 선택.
   Core 계약과 기존 GLES/Vulkan/DYNAMIC 유지. concrete EGL/native FD/RTTI는 native 격리.
2. 웹 별도 메모리 + 보존이 필요한 버퍼 소유 CPU shadow.
   native Memory2/3 유지, Memory4 없음. callback/partial/uniform 보존·오류·수명 검증.
3. 브라우저 main = DALi 이벤트, 별도 pthread Worker = 업데이트·업로드·렌더.
   DOM 경계 분리, main ready/stop blocking 제거. 이벤트 Worker는 플레이그라운드 격리 후보.
4. GitHub Pages + SW 우선 검증. 실패/과도한 비용 시 직접 호스팅 검토.
   다른 관리형 호스팅 금지. 이번 Pages 배포·서버 구축 금지.

첫 작업은 P2-b-HANDOFF 4번 웹 메모리야.
MapBufferRange factory 환경 분리, WebBufferMemory와 버퍼 소유 shadow 구현.
16-byte 패턴 중간 4-byte 갱신, callback 0/부분/전체, nonzero offset, CPU/GPU uniform,
bounds/overflow/할당 실패/double unmap/grow/recycle/종료 수명을 확인해줘.
CPU 단위 검사만으로 GPU 보존 통과 처리하지 말고 P3 실제 검증으로 남겨줘.
Core 호출부의 실패 처리에 공통 계약 변경이 필요하면 먼저 멈추고 보고해줘.

이후 wrapper/capability → shader cache 경계 → 실제 host/D3 thread → 기본 WebGL2 strict 생산 링크.
정식 source closure와 generated source 정리도 마무리해줘.
mock Controller, 빈 GL/EGL/sync, undefined 억제로 성공 처리 금지.
FULL_ES3 링크 지원은 runtime/데이터 보존 근거가 아니야.
실제 Core·생산 Controller·자원·명령·host 포함 strict exit 0,
manifest/nm/map·명령·버전·개별 종료 코드를 남겨줘.
생산 링크 성공 전 상위 P2 완료 선언 금지. 실제 픽셀은 P3 별도 게이트.
공통/native GLES/Vulkan/DYNAMIC 회귀를 수행하고 못 한 검사는 이유/영향 범위를 명시해줘.
공통 계약 변경·전체 플랫폼 포팅·실험 확대·합의 방향 변경이 필요하면 먼저 멈추고 보고해줘.
구현 단위가 커지면 검증 가능한 지점에서 중간 인수인계를 남겨줘.

최초 pthread/browser 단위에는 로컬 헤더/SW 비교 probe 준비,
별도 승인 후 실제 Pages 최초/재방문·업데이트/해제·하위 경로 검증을 유지해줘.
P3/P4 플레이그라운드 격리 조기 검증, P4 종료 전 무한 루프 중 응답성과
전체 pthread/Canvas 종료·재생성 결과를 기록해줘.

원본 사용자 변경·P0·SDK·R2/P1/P2-a/context/sync 증거·기존 산출물 보존.
upstream 갱신·Pages 배포·서버 구축 금지. 별도 build/log, 최대 -j6.
개인 SDK 절대 경로 필수화 금지. SDK·산출물·프로필·대용량 로그 commit 금지.
로컬 commit·대상 원격 push 허용, force push 금지.
모든 생성 commit은 상위 AGENTS.md의 현재 세션 실제 모델 AI co-author 규칙을 지켜줘.
PLAN·STATE·JOURNAL·HANDOFF·다음 프롬프트를 갱신하고
특이 사항·검증 결과·미검증 범위·다음 진행 가능 여부를 보고해줘.
```
