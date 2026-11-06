# P3 운영 캐시 정책 검토 — 제안·미합의

2026-11-06. 검토 단위 완료이며 정책 채택·구현·runtime 검증 완료가 아니다. 시작 HEAD와 실제 origin/main은 `f81276e13314ae68170b09767782abd229a4a958`, main clean. 상위 AGENTS 및 NEXT-CONTEXT/HANDOFF의 필수 문서·lock과 현재 bootstrap/SW를 확인했다. sandbox DNS 실패 후 권한 확장 읽기 조회로 원격을 확인했다.

## 비교

| 관점 | Network-only | 버전별 불변 자산 묶음 + 명시적 캐시 |
|---|---|---|
| 버전 혼용 | 캐시를 끄는 것만으로는 요청 사이 배포 변경을 막지 못함. 동일 릴리스 URL 고정이 별도로 필요 | HTML/bootstrap·JS/Wasm/pthread Worker를 한 manifest에 고정. 완료된 묶음만 실행하도록 설계 가능 |
| 업데이트 | 구현은 단순하나 매 요청의 연결/서버 상태에 의존. no-store도 CDN 원본 최신성이나 여러 파일의 원자성을 보장하지 않음 | 새 묶음을 별도 staging에 준비·검증한 뒤 다음 실행에서 선택. 실행 중인 탭은 기존 릴리스 유지 |
| 실패 복구 | 연결 복구 후 같은 버전으로 명시적 재시도. 이전 파일이 서버에서 사라지면 복구 불가 | 새 다운로드 실패 시 기존 완료 묶음 보존. 이전 완료 릴리스 전체로 명시적 복귀 가능 |
| 비용 | 재다운로드 비용, 대형 컴파일러 준비 지연 | 저장 용량·eviction·불완전 staging·구버전 회수 계약 필요 |

현재 `production-sw.js`는 CacheStorage 없이 `fetch(event.request)`에 격리 헤더만 추가한다. 이는 HTTP cache 우회를 보장하는 엄격한 network-only가 아니다. `production.js`도 고정 `./runtime/` 경로이며 릴리스 pinning이 없다. 기존 테스트의 HTML no-store는 reload 계수용일 뿐 운영 정책이 아니다.

## 권장안 — 승인 후 구체화

1. 버전별 불변 URL과 manifest를 기준으로 묶음 전체를 선택한다. 버전은 SDK 숫자만이 아니라 실제 build/release ID와 파일 SHA256을 포함한다. bootstrap·JS·Wasm·Worker 및 전이 의존성을 같은 릴리스로 고정한다. E/C는 별도 variant로 식별하고 섞지 않는다.
2. 새 묶음은 별도 staging cache에 내려받아 필수 파일의 상태·hash·MIME을 검증한 뒤 완료 표시를 기록한다. loader는 완료 표시와 실제 파일 존재를 확인한 묶음만 사용한다. CacheStorage 자체에 여러 cache를 아우르는 트랜잭션이 있다고 가정하지 않는다. 오류 응답이나 다른 릴리스 파일로 누락을 메우지 않는다.
3. 실행은 시작 시 릴리스에 고정한다. 업데이트는 다음 실행/명시적 재시작에서 적용하며 실행 중 자동 reload를 하지 않는다. SW 코드 업데이트와 앱 자산 업데이트를 구분한다. SW URL은 안정적으로 유지하고, 현재 fixture의 무조건 skipWaiting/claim을 운영 업데이트 계약으로 그대로 채택하지 않는다. 최초 격리 제어 확보와 기존 탭 업데이트를 구분해 설계·검증한다.
4. 다운로드 실패·quota 부족이면 새 묶음을 실행하지 않고 기존 완료 묶음을 유지한다. 파일 유실/오염은 같은 불변 URL에서 검증해 복구하거나 오류 후 명시적 재시도한다. rollback은 이전 완료 묶음 전체로 새 실행하며 파일별 fallback은 금지한다. 캐시는 영구 보관이 아니므로 복구 가능한 서버 측 이전 릴리스 보존도 필요하다. 보존 개수·기간·용량과 다중 탭 사용 중 cache 회수 규칙은 구현 전 확정한다. cache 이름은 origin 내 다른 Pages 프로젝트와 구별한다.
5. 원본 Emception3.1.24·헤더·라이브러리·DALi archive/객체·링크 옵션은 동일 manifest의 호환성 계약에 연결한다. P3에서는 작은 E runtime 묶음으로 먼저 검증하고 대형 컴파일러 패키지 구현은 P8 범위를 유지한다. 브라우저 생성 JS/Wasm/Worker는 컴파일 작업별 별도 묶음으로 취급하며 정적 배포 cache와 섞지 않는다. 전체 오프라인 컴파일 지원을 이번 정책만으로 약속하지 않는다.

최신 릴리스 안내는 네트워크 재검증 대상으로, 실제 실행 자산은 불변 URL 대상으로 구분하는 안이다. 안내 조회 실패 때 이전 묶음을 쓸지는 명시적 사용자 선택으로 드러낸다. 최초 비제어 navigation은 SW가 응답을 바꿀 수 없으므로 진입 HTML도 일관된 릴리스 참조를 갖춰야 한다. 실제 Pages/CDN 응답과 구버전 파일 보존 가능성은 별도 승인 검증 대상이다.

## 승인 후 제안 검증 범위

서로 다른 A/B 묶음으로 정상 업데이트, 부분 다운로드/404/503/hash 불일치, 이전 탭의 늦은 Worker 로딩, 실행 중 업데이트·다중 탭, cache 유실/quota 실패·재시도·전체 rollback을 검사한다. warm cache/HTTP cache 개입과 브라우저 재시작도 구분한다. 기존 Firefox9회 복구 및 직접 헤더 E 픽셀 회귀를 보존한다. 새 정책은 아직 브라우저에서 실행하지 않았으며 Chromium·전체 offline·실제 Pages도 미검증이다.

## 근거·특이 사항

- [Request.cache](https://developer.mozilla.org/en-US/docs/Web/API/Request/cache): default는 HTTP cache 사용 가능, no-store는 브라우저 HTTP cache 조회·저장을 우회. 캐시 부재와 버전 일관성은 별개다.
- [SW lifecycle](https://web.dev/articles/service-worker-lifecycle): skipWaiting으로 새 SW가 이전 페이지의 후속 요청을 처리할 수 있음. 안정적인 SW URL 및 origin 내 프로젝트별 cache 이름 분리 참고.
- [Cache API](https://developer.mozilla.org/en-US/docs/Web/API/Cache): 명시적 저장/삭제와 quota·eviction 고려 필요.

특이 사항은 현재 SW의 HTTP cache 개입 가능성과 즉시 활성화 정책이다. 기존 복구 결과를 무효화하는 새 runtime 실패를 재현한 것은 아니다. 위 권장안은 문서·코드 및 공식 API 설명을 바탕으로 한 설계 제안이다. 정책 구현은 사용자 합의 전 중단한다. 기본 E·독립 C, 원본 Emception+Emscripten3.1.24, 서버 없는 브라우저 컴파일·Pages+SW 우선 및 P1 D4 중단 조건을 유지한다. 공통/native·SDK·lock·SW/테스트 코드 변경, 빌드·브라우저 실행·배포 없음.
