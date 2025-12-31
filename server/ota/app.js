const fileEl = document.getElementById("file");
const versionEl = document.getElementById("version");
const btnUpload = document.getElementById("btnUpload");
const btnRefresh = document.getElementById("btnRefresh");
const btnSetProd = document.getElementById("btnSetProd");
const tbody = document.getElementById("tbody");
const logEl = document.getElementById("log");
const statusEl = document.getElementById("status");

function setStatus(msg, isError = false) {
    statusEl.textContent = msg;
    statusEl.style.color = isError ? "rgba(255,120,120,0.95)" : "rgba(255,255,255,0.70)";
}

function log(msg) {
    logEl.textContent += msg + "\n";
    logEl.scrollTop = logEl.scrollHeight;
}

function fmtBytes(n) {
    if (!Number.isFinite(n)) return "";
    const units = ["B", "KB", "MB", "GB"];
    let v = n, i = 0;
    while (v >= 1024 && i < units.length - 1) { v /= 1024; i++; }
    return `${v.toFixed(v >= 10 || i === 0 ? 0 : 1)} ${units[i]}`;
}

function escapeHtml(s) {
    return String(s).replace(/[&<>"']/g, c => ({
        "&": "&amp;", "<": "&lt;", ">": "&gt;", "\"": "&quot;", "'": "&#39;"
    })[c]);
}

function selectedFilename() {
    const el = document.querySelector('input[name="pick"]:checked');
    return el ? el.value : null;
}

async function fetchList() {
    setStatus("Loading list…");
    tbody.innerHTML = `<tr><td colspan="6" class="muted">Loading…</td></tr>`;
    try {
        const res = await fetch("/api/ota/firmware");
        const data = await res.json();

        if (!res.ok || !data.ok) throw new Error(data.error || `HTTP ${res.status}`);

        if (!data.files.length) {
            tbody.innerHTML = `<tr><td colspan="6" class="muted">No firmware uploaded yet.</td></tr>`;
            setStatus("Ready.");
            return;
        }

        tbody.innerHTML = data.files.map((f, idx) => {
            const checked = f.isProduction || (data.production == null && idx === 0);
            return `
        <tr>
          <td><input class="radio" type="radio" name="pick" value="${escapeHtml(f.name)}" ${checked ? "checked" : ""}></td>
          <td><code>${escapeHtml(f.name)}</code></td>
          <td>${escapeHtml(f.version || "")}</td>
          <td>${escapeHtml(fmtBytes(f.size))}</td>
          <td>${escapeHtml(new Date(f.mtime).toLocaleString())}</td>
          <td>${f.isProduction ? `<span class="badge prod">production</span>` : `<span class="badge">—</span>`}</td>
        </tr>
      `;
        }).join("");

        setStatus("Ready.");
    } catch (err) {
        tbody.innerHTML = `<tr><td colspan="6" class="muted">Failed to load.</td></tr>`;
        setStatus(`Error: ${err?.message || err}`, true);
        log(`❌ ${err?.message || err}`);
    }
}

btnRefresh.addEventListener("click", fetchList);

btnUpload.addEventListener("click", async () => {
    const file = fileEl.files?.[0];
    const version = (versionEl.value || "").trim();

    if (!file) return setStatus("Please choose a firmware file first.", true);
    if (!version) return setStatus("Please enter a version string.", true);

    setStatus("Uploading…");
    log(`⏳ Uploading ${file.name} (version: ${version})`);

    try {
        const fd = new FormData();
        fd.append("file", file);
        fd.append("version", version);

        const res = await fetch("/api/ota/firmware", { method: "POST", body: fd });
        const data = await res.json().catch(() => ({}));

        if (!res.ok || !data.ok) throw new Error(data.error || `HTTP ${res.status}`);

        log(`✅ Uploaded: ${data.file} (version: ${data.version})`);
        setStatus("Upload complete ✅");

        fileEl.value = "";
        // keep version field as-is on purpose

        await fetchList();
    } catch (err) {
        setStatus(`Upload failed: ${err?.message || err}`, true);
        log(`❌ Upload failed: ${err?.message || err}`);
    }
});

btnSetProd.addEventListener("click", async () => {
    const filename = selectedFilename();
    if (!filename) return setStatus("Select a firmware file first.", true);

    setStatus("Setting production…");
    log(`⏳ Setting production: ${filename}`);

    try {
        const res = await fetch("/api/ota/production", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ filename }),
        });
        const data = await res.json().catch(() => ({}));

        if (!res.ok || !data.ok) throw new Error(data.error || `HTTP ${res.status}`);

        log(`✅ Production set to: ${data.production}`);
        setStatus("Production updated ✅");
        await fetchList();
    } catch (err) {
        setStatus(`Failed: ${err?.message || err}`, true);
        log(`❌ Failed: ${err?.message || err}`);
    }
});

// Initial load
fetchList();