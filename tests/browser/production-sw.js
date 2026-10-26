// Test-only response headers; cache/update policy remains a separate gate.
self.addEventListener('install', event => event.waitUntil(self.skipWaiting()));
self.addEventListener('activate', event => event.waitUntil(self.clients.claim()));
self.addEventListener('fetch', event => {
  if (new URL(event.request.url).origin !== self.location.origin) return;
  event.respondWith((async () => {
    const response = await fetch(event.request);
    const headers = new Headers(response.headers);
    headers.set('Cross-Origin-Opener-Policy', 'same-origin');
    headers.set('Cross-Origin-Embedder-Policy', 'require-corp');
    return new Response(response.body, {status: response.status, statusText: response.statusText, headers});
  })());
});
