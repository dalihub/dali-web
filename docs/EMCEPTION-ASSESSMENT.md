# 서버 없는 C++ 플레이그라운드: Emception 검증

최신 후속 결과: [3.1.24로 DALi를 재빌드한 정렬 검사](EMCEPTION-3.1.24-ALIGNMENT.md)에서 전체 브라우저 strict 링크와 새 사용자 C++ 함수 실행에 성공했다. 아래는 6.0.13 산출물을 원본 3.1.24와 혼용했던 당시 기록이다.

2026-10-26 (Asia/Seoul). 사용자 요청으로 추천한 원본 `jprendes/emception`을 검증했다. **브라우저 내 C++ 컴파일·링크·실행은 확인했다. 현재 DALi 전체 생산 산출물과 원본 Emception 배포물의 직접 결합은 실패했다.** 따라서 구현 참고/도구 기반 후보로는 유효하지만 현재 상태 그대로 채택 가능한 SDK로 판정하지 않는다.

## 범위와 대상

별도 운영 서버 없는 GitHub Pages 방향을 유지한다. 공개 Emception 소스는 commit `5ff1975a5d286b200edc380ba8cc0de94d5c0cdc`를 확인했다. docker 및 Emscripten package recipe는 3.1.24를 고정하고, LLVM은 `d5a963ab8b40fcf7a99acd834e5f10a1a30cc2e5`를 선택한다. 원본 LICENSE는 MIT OR Apache-2.0이며 포함 도구·라이브러리의 라이선스는 별도다.

공식 데모에서 실제 실행한 em++ 버전은 **3.1.24-git**이다. 소스 master와 배포 파일이 같은 commit이라고 가정하지 않았으며 배포 파일 hash를 별도 기록했다. 현재 dali-web lock은 **Emscripten 6.0.13**이다. npm의 동명 또는 다른 포크 제품을 원본의 검증 결과로 취급하지 않았다.

## 실제 결과

| 검사 | 결과 | 의미 |
|---|---|---|
| 공식 Pages의 C++17 vector/iostream 컴파일·링크 | 0 | C++ 소스를 브라우저 컴파일러가 처리 |
| 문법 오류 소스 | 1, 소스 위치·진단 반환 | 오류 전달 가능 |
| pthread 생성 코드 컴파일·링크 | 0 | pthread 산출물 생성 가능, pthread 실행 검증 아님 |
| pthread+WebGL2+OffscreenCanvas strict 링크 | 0 | FULL_ES3=0·ABORTING_MALLOC=0 유지, GL 실행/픽셀 검증 아님 |
| 생성된 STL 프로그램의 Worker 실행 | RESULT:3, exit 0 | 컴파일 결과 실행까지 확인 |
| 실제 DALi Vector2 헤더를 사용하는 사용자 소스 | 컴파일·현재 Vector2 객체 링크 0, LENGTH:5·exit 0 | 실제 기존 DALi 객체와 작은 앱의 브라우저 내 연결 확인 |
| 현재 host 객체의 relocatable 링크 | 0 | 구형 링커가 해당 객체 형식을 읽음. 전체 ABI 호환성 아님 |
| 실제 생산/host 35객체 + 현재 Core archive의 strict 링크 | **1** | 구형 libc++에 std::__2::__hash_memory 정의 없음 |

생산 실패는 환경에 없는 라이브러리 때문에 실패한 중간 시도와 구분했다. 최종 assessment-006에서는 필요한 정적 자산을 모두 제공한 상태에서 `_ZNSt3__213__hash_memoryEPKvm` 미해결로 중단됐다. 현재 Core archive에는 이 심볼 참조가 20개 있고 Emception의 libc++-mt-noexcept archive에는 정의가 없음을 llvm-nm으로 확인했다. 이 첫 오류가 전체 호환성 차이의 전부라고 단정하지 않는다. 후속 JS compiler 예외도 원문 로그에 보존했다.

최종 생산 검사에 입력한 객체·Core archive는 기존 production-003과 동일한 hash이다. 기존 P2의 6.0.13 strict 링크 성공은 유효하며 이번 다른 toolchain 결합 실패와 혼동하지 않는다. undefined 오류를 끄거나 임의 구현으로 성공 처리하지 않았다.

## 특이 사항

1. 공식 데모 페이지는 검사 시 crossOriginIsolated=false였다. pthread 링크 성공을 그 페이지에서 pthread 실행이 된다는 뜻으로 확대하지 않는다. 로컬 fixture는 COOP/COEP를 제공해 crossOriginIsolated=true를 확인했으나 실제 pthread/Worker GL 실행 검사는 이번에 수행하지 않았다.
2. 제3자 페이지로 로컬 DALi 파일을 전달하는 초기 검사는 자동 승인 검토가 거부했고 실행하지 않았다. 대안으로 공개 도구만 내려받아 CSP·외부 차단 프록시가 적용된 loopback fixture에서 DALi 입력을 처리했다. 별도 운영 서버나 외부 컴파일 API를 사용하지 않았다.
3. 초기 로컬 harness 구문 오류 및 필요한 debug/libnoexit 패키지 누락을 수정하고 재검사했다. 이들 중간 실패도 보존했다. 원본 Emception 소스·compiler를 수정한 것은 아니다.
4. LLVM_ENABLE_THREADS=OFF는 컴파일러 자체의 빌드 옵션이다. 생성하는 앱의 pthread 지원 불가 근거로 해석하지 않는다. 실제 pthread 링크는 성공했다.
5. 작은 로컬 STL 컴파일은 약 1.7초, Vector2는 약 0.6초였으나 한 환경의 관찰치다. 최초 전체 다운로드·모바일·대규모 사용자 소스·메모리 최고치·서비스 성능 목표 충족은 측정하지 않았다. 내려받은 선택 자산은 배포 최적화 결과나 최소 크기 추정이 아니다.

## 판정과 다음에 필요한 작업

**서버 없는 방식의 실현 가능성은 실제 동작으로 확인했다. 원본 Emception을 그대로 현재 DALi에 연결하는 것은 불가하다.** 현재 SDK에 맞춘 browser toolchain을 확보하는 것이 다음 기술 과제다. Emception을 6.0.13 계열로 갱신하거나, 같은 버전에 맞는 유지보수 포크를 검증하는 방향이 우선이며 이번에는 선택·이식·SDK 변경을 실행하지 않았다. 구형 compiler를 이유로 DALi SDK를 임의로 내리지 않았다.

P8의 별도 컴파일 서버 전제는 사용자가 재확인한 방향과 맞지 않는다. 최종 compiler 선택과 구현 계획 갱신은 별도 논의 대상이며, 이번 검증을 P8 완료로 선언하지 않는다. P3 GPU/readback·P3/P4 격리·P4 전체 종료/재생성 게이트도 그대로 남는다.

[명령·입력·개별 결과와 재현 기록](evidence/emception/README.md). 생산 소스·SDK·lock 변경, upstream 갱신, commit/push, Pages 배포 없음. 로컬 fixture와 모든 검증 브라우저는 종료했다.

## 확인한 원본

- [Emception source](https://github.com/jprendes/emception/tree/5ff1975a5d286b200edc380ba8cc0de94d5c0cdc)
- [고정 SDK recipe](https://github.com/jprendes/emception/blob/5ff1975a5d286b200edc380ba8cc0de94d5c0cdc/packs/emscripten/make.sh)
- [LLVM build recipe](https://github.com/jprendes/emception/blob/5ff1975a5d286b200edc380ba8cc0de94d5c0cdc/build-llvm.sh)
- [공식 Pages 데모](https://jprendes.github.io/emception/)
