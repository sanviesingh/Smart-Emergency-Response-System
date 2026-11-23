const API = "";
const $ = (id) => document.getElementById(id);

function toast(message) {
  const el = $("toast");
  el.textContent = message;
  el.classList.add("show");
  setTimeout(() => el.classList.remove("show"), 2800);
}

async function api(path, options = {}) {
  const response = await fetch(API + path, {
    headers: { "Content-Type": "application/json" },
    ...options
  });
  const data = await response.json();
  if (!response.ok && !data.message) throw new Error(data.error || "Request failed");
  return { response, data };
}

function priorityClass(level) { return "p" + level; }

async function loadEmergencies() {
  const { data } = await api("/api/emergencies");
  $("totalCount").textContent = data.length;
  $("criticalCount").textContent = data.filter(e => e.severity === 4).length;
  $("pendingCount").textContent = data.filter(e => e.status === "Pending").length;

  const tbody = $("emergencyTable");
  if (!data.length) {
    tbody.innerHTML = '<tr><td colspan="5" class="empty">No emergencies registered yet.</td></tr>';
    return;
  }
  tbody.innerHTML = data.map(e => `
    <tr>
      <td>#${e.id}</td>
      <td><strong>${escapeHtml(e.type)}</strong><br><small>${escapeHtml(e.location)}</small></td>
      <td><span class="pill ${priorityClass(e.severity)}">${escapeHtml(e.severityName)}</span></td>
      <td><span class="status ${e.status === "Assigned" ? "assigned" : ""}">${escapeHtml(e.status)}</span></td>
      <td><button class="assign-btn" data-id="${e.id}" ${e.status !== "Pending" ? "disabled" : ""}>Assign</button></td>
    </tr>`).join("");
  tbody.querySelectorAll(".assign-btn").forEach(btn =>
    btn.addEventListener("click", () => assignEmergency(Number(btn.dataset.id))));
}

function escapeHtml(value) {
  return String(value).replace(/[&<>"']/g, ch => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"
  }[ch]));
}

async function loadResources() {
  const { data } = await api("/api/resources");
  $("resourceCount").textContent = data.filter(r => r.available).length;
  $("resourceList").innerHTML = data.map(r => `
    <div class="resource">
      <div><strong>${escapeHtml(r.name)}</strong><small>${escapeHtml(r.type)} · Zone ${r.zone}</small></div>
      <span class="availability ${r.available ? "" : "busy"}">${r.available ? "AVAILABLE" : "BUSY"}</span>
    </div>`).join("");
}

async function refreshAll() {
  try {
    await Promise.all([loadEmergencies(), loadResources()]);
    $("connectionStatus").textContent = "Backend connected";
  } catch (error) {
    $("connectionStatus").textContent = "Backend disconnected";
    toast("Cannot connect to C++ backend. Start lifeline.exe first.");
  }
}

$("emergencyForm").addEventListener("submit", async event => {
  event.preventDefault();
  const payload = {
    type: $("type").value,
    location: $("location").value.trim(),
    severity: Number($("severity").value),
    description: $("description").value.trim()
  };
  try {
    const { data } = await api("/api/emergencies", {
      method: "POST", body: JSON.stringify(payload)
    });
    if (!data.success) throw new Error(data.message);
    toast(`Emergency #${data.id} registered.`);
    $("emergencyForm").reset();
    await refreshAll();
  } catch (error) { toast(error.message); }
});

async function assignEmergency(id) {
  try {
    const { data } = await api(`/api/assign/${id}`, { method: "POST", body: "{}" });
    if (!data.success) {
      toast(data.message);
      return;
    }
    $("assignmentResult").innerHTML = `
      <div class="assignment-card">
        <h3>✓ Resource assigned</h3>
        <p><b>Resource:</b> ${escapeHtml(data.resourceName)} (#${data.resourceId})</p>
        <p><b>Destination:</b> ${escapeHtml(data.hospitalName)}</p>
        <p><b>Shortest route:</b> ${escapeHtml(data.route)}</p>
        <p><b>Estimated network distance:</b> ${data.distanceKm} km</p>
        <small>Distance is based on the illustrative sample graph, not live road data.</small>
      </div>`;
    toast(data.message);
    await refreshAll();
  } catch (error) { toast(error.message); }
}

$("refreshBtn").addEventListener("click", refreshAll);
refreshAll();
