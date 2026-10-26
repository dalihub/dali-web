# 다음 컨텍스트용 프롬프트

```text
DALi 브라우저 포팅을 이어가자.
최신 후속 결과는 docs/P3-INITIAL-RESULTS.md와 docs/evidence/p3-initial/README.md야.
3.1.24 전용 toolchain.emception-3.1.24.lock.json과 scripts/env-emception-3.1.24.sh를 추가했어.
기존 lock6.0.13/SDK/자동 탐색을 보존했고 명시 profile은 compiler revision을 검사해.
Core CPU uniform persistent mapping 재사용과 Web UNIFORM_NAME_LENGTH 호환 조회를 수정했어.
최종 build/p3/pixels-3124-002는 새 Core333·생산35 compile/nm·strict0,
controller-3124-003 CPU18과 gl-3124-003 CPU5도0이야.
실제 Firefox157.0.1의 browser-pixels-headers-001/sw-001 모두 두 frame에서
배경13,25,51,255·사각형229,76,25,255·GL error0, ready→stopped를 확인했어.
SW는 /probe/ 하위경로 첫 isolation=false→제어/reload 후true야. Pages는 아니야.
DALI_WEB_TEST_READBACK은 기본OFF이고ON에서만 두 frame readback이 들어가.
production-final-001은 같은 최종 Core/객체에 계측 없는 host를 별도로 링크했고
browser-final-001도 직접 헤더에서 오류 없이 ready→stopped, 종료0이야.
최초 GL 오류/trap, 다음 CPU 중복 lock 실패, 수정 전 Core 회귀 실패를 보존했어.
pixels-3124-001은 빌드 중 Core 수정이 있어서 최종 근거로 사용하지 마.
P3 전체 완료는 아니야. 다음은 GPU16-byte/중간4-byte·callback0/부분/전체·
nonzero offset·CPU/GPU uniform·두 frame·grow/recycle·수명의 종합 GPU 검사야.
현재 픽셀/CPU 검사로 그 게이트를 통과 처리하지 마.
이벤트 Worker 격리·SW 재방문/업데이트/해제/실패·Pages 승인·전체 종료/재생성도 남아.
일반 shader/GL 실패 때 reflection 안전성은 아직 검증하지 않았어.
Core 내부 변경은 native에도 적용돼. native configure3개는 compiler 부재로1,
native 관련13개 Emscripten compile/nm은0이고 native runtime은 미검증이야.
아래는 이전 인계 기록이야. 프로필/CPU 재검증과 최초 픽셀 작업은 위 최신 결과를 따라줘.

작업 repo: /home/jy/work/dali/dali-web
현재 기록: dali-web/docs
과거 증거·SDK: /home/jy/work/dali/webassembly-probe
원격: https://github.com/dalihub/dali-web

2026-10-26 사용자 승인으로 원본 Emception 3.1.24와 같은 SDK 계열 DALi 조합을 채택했어.
PLAYGROUND-COMPILATION.md의 결정과 다음 결과를 확인해줘:
EMCEPTION-3.1.24-ALIGNMENT.md와 evidence/emception-3.1.24-alignment/README.md,
EMCEPTION-ASSESSMENT.md와 EMCEPTION-5.0.7-FOLLOWUP.md.
플레이그라운드는 별도 운영 서버 없이 브라우저에서 사용자 C++를 컴파일하는 방향이야.
원본 Emception3.1.24에 맞춰 별도 SDK로 실제 DALi Core333·생산/host35개를
재빌드했고 로컬/브라우저 strict 링크0, 새 사용자 C++ 컴파일/링크0,
Vector2와 문자열 unordered_map 사용자 함수 반환5를 확인했어.
최종 native-002와 browser-001 증거를 봐줘. 첫 빌드의 SDK config/cache 재초기화
실패는 보존했고, 설정 고정 후 소스 수정 없이 재빌드했어.
직접 헤더와 헤더 없는 SW의 모듈 로딩·공유 메모리·동일 함수 실행도 통과했지만
dali_web_start는 호출하지 않았고 GPU/Worker GL/실제 Pages/전체 종료는 미검증이야.
GameGuild 4.5.0/emsdk5.0.7은 공식 구성 C++/STL 실행은 성공했지만
일반 em++ JS 생성 경로가 생략되어 후보 게이트는 미통과야.
사전 로딩 보강으로 debug/WebGL/pthread 링크 정체를 넘겨도 JS 생성 실패야.
기존 lock/SDK6.0.13과 생산 입력497개는 과거 재현용으로 보존했어.
다음 작업 기준은 승인된 3.1.24야. 후보 선택이나 서버 없는 방향을 재승인받지 마.
GameGuild JS 실행 계층 포팅이나 별도 운영 컴파일 서버로 방향을 바꾸지 마.
자동 기본 SDK 선택은 아직 바꾸지 않았으니 먼저 기존 lock을 덮어쓰지 않는
별도 3.1.24 프로필/lock과 명시 도구 선택을 정리하고 CPU18/5를 재실행해줘.
로컬 env는 logs/emception/alignment-3124-001/env.sh, 성공 빌드는
build/emception/alignment-3124-002야. 수동 SDK tools/emsdk-3.1.24에는
emsdk_env.sh가 없어. BUILDING의 EM_CONFIG/EM_CACHE와 명시 도구 override를 따라줘.
EMCC_CORES=6을 유지하고 6.0.13 객체를 혼용하지 마. 새 build/log를 사용해줘.

상위 AGENTS.md와 README, STATE·HANDOFF·PLAN,
P2-b-PRODUCTION-RESULTS.md와 evidence/p2-b-production/README.md,
P2-b-WEB-MEMORY-RESULTS.md와 evidence/p2-b-web-memory/README.md,
P2-b-CORE-FAILURE-RESULTS.md, P1-EXECUTION-CONTRACT.md,
P2-a-MANIFEST·P2-a-RESULTS·P2-b-HANDOFF·P2-b-SYNC-RESULTS,
BUILDING·UPSTREAMS·MIGRATION, upstreams.lock.json·toolchain.lock.json을 먼저 읽어줘.
P2-b-MEMORY-BLOCKER는 승인 전 역사 기록이며 승인 대기 상태가 아니야.
Core 실패 처리와 Web fail-stop 설계는 이미 승인했으니 재승인받지 마.

HEAD는 abf62214b23c651f06536138cb8b6e0c0fddbfb7이고,
기존 Core internal 변경과 Web 메모리·wrapper/cache·실제 D3 host·검사 코드가
working tree에 있어. commit/push하지 않았어. 실제 HEAD/status/원격을 확인하고 보존해줘.
지난 원격 GET main은 a68e3f00f0c7752561137c179500b004b930285e야.

P2-b 구현과 P2 기본 WebGL2 strict 링크 게이트를 완료했어.
실제 Core·생산 Controller·자원·명령·host가 링크됐고 mock/빈 GL/undefined 억제는 없어.
최종 production-003 명령·입력 hash·manifest/nm/map은 생산 증거를 확인해줘.
controller-007은 실제 생산 Buffer/Controller/Core uniform + GL 주입 CPU18모드0,
gl-unit-002는 wrapper/capability CPU5모드0이야. GPU/브라우저 증거가 아니야.
source 회귀·closure 검사는0, native configure3개는 compiler 부재로1,
native 추출/공통13개 Emscripten compile/nm은0이야. native build/runtime은 미검증이야.

native Memory2/3 유지, Memory4 없음, 공개 Graphics Controller/Memory API 유지.
Web lazy shadow·offset/bounds·참조 수명·detach·실패 latch·제출 gate가 있어.
실제 factory allocation/grow/recycle·create/discard·Core uniform·live Memory shutdown을 검사했어.
shutdown custom allocator 해제 누락을 재현·수정했고 SyncDeleter에도 적용했어.
Web 생성 재시도 없음, native 재시도 유지. Unlock/Unmap은 void라 Core bool은 GL/GPU 성공이 아니야.

WebGL2 wrapper와 binary cache를 native/Web 파일로 분리했어.
cmake/web이 정식 최소 생산 target이고 전체 Tizen Adaptor WEB profile은 계속 거부해.
namespace/invoke-method와 native cache/pkg-config 생성물은 binary directory를 사용해.
-sABORTING_MALLOC=0을 최종 host 링크에도 유지해. 전체 Core/STL OOM까지 처리한 것은 아니야.

실제 host는 main 이벤트·DOM·Core/TLS와 별도 pthread update/upload/render야.
main8ms/Worker4ms mailbox polling으로 ready/failure/stop을 비동기 전달해.
최대 한 frame, event 정리→owner Controller/fence shutdown→main Core 해제→
owner context 폐기/pthread 종료→main 비차단 reap/pooled Worker 종료 순서야.
실패 후 새 Module/Worker/Canvas, 자동 재시도 없음.
브라우저 비동기 종료·성능·GPU 픽셀은 아직 runtime 검증하지 않았어.

다음은 P3 실제 browser 초기화·DALi 사각형/배경 픽셀·GL 오류 검사야.
16-byte/중간4-byte·callback0/부분/전체·nonzero offset·CPU/GPU uniform·두 frame·
grow/recycle·수명을 실제 GPU readback/픽셀로 검증해줘.
로컬 loopback 임시 파일 제공으로 실제 pthread 렌더링의 헤더/SW 비교를 진행하고,
기존 모듈 로딩 성공을 Worker GL/GPU 성공으로 대신하지 마.
SW 재방문·업데이트·해제/실패와 하위 경로 검증도 게이트에 남겨두고,
실제 Pages 검증은 별도 승인 게이트로 유지해줘.
P3/P4 이벤트 Worker 플레이그라운드 격리 조기 검사와
P4 종료 전 무한 루프 응답성·전체 pthread/Canvas 종료/재생성을 유지해줘.
GitHub Pages+SW 우선, 실패/과도한 비용 시 직접 호스팅 검토. 다른 관리형 호스팅 금지.

원본 사용자 변경·P0·SDK·과거 증거·lock·산출물을 보존해줘.
upstream 갱신·Pages 배포·서버 구축 금지. 별도 build/log, 빌드 최대-j6.
개인 SDK 절대 경로를 필수화하지 말고 SDK·산출물·프로필·큰 로그는 commit하지 마.
로컬 commit·대상 원격 push 허용, force push 금지. 실제 세션 모델 co-author 규칙 준수.
공통/native GLES/Vulkan/DYNAMIC 회귀와 못 한 검사의 이유·영향을 명시해줘.
승인 범위를 넘는 공통/public 계약 변경·전체 플랫폼 포팅·실험 확대·합의 방향 변경만 중단·보고해줘.
PLAN·STATE·JOURNAL·HANDOFF·다음 프롬프트를 갱신하고 구현·특이 사항·검증·미검증·단계 상태를 보고해줘.
```
